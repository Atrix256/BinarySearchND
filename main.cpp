#include <stdio.h>
#include <random>
#include <vector>
#include <algorithm>

// helpful for debugging
#define DETERMINISTIC() false

// Just binary search
bool Search1D(const int* values, int startX, int endX, int key)
{
	// If our search area has zero size, bail out
	if (startX == endX)
		return false;

	int elementIndex = (startX + endX) / 2;
	int element = values[elementIndex];

	if (element == key)
		return true;

	// if the element we looked at is less than the key, look at the right section
	if (element < key)
		return Search1D(values, elementIndex + 1, endX, key);
	// else the element is greater than the key, so look at the left section
	else
		return Search1D(values, startX, elementIndex, key);
}

// Divide and conquer
bool Search2D(const int* values, int width, int startX, int startY, int endX, int endY, int key)
{
	// If our search area has zero area, bail out
	if (startX == endX || startY == endY)
		return false;

	int elementIndexX = (startX + endX) / 2;
	int elementIndexY = (startY + endY) / 2;
	int elementIndex = elementIndexY * width + elementIndexX;
	int element = values[elementIndex];

	if (element == key)
		return true;

	// if the element we looked at is less than the key, the key is not <= this location on x and y. remove that <= block
	if (element < key)
	{
		// the right side
		if (Search2D(values, width, elementIndexX + 1,            startY,              endX, endY, key))
			return true;

		// the bottom side
		if (Search2D(values, width,            startX, elementIndexY + 1, elementIndexX + 1, endY, key))
			return true;
	}
	// else the element is greater than the key, so the key is not >= this location on x and y. remove that >= block.
	else
	{
		// the left side
		if (Search2D(values, width,        startX, startY, elementIndexX,          endY, key))
			return true;

		// the top side
		if (Search2D(values, width, elementIndexX, startY,          endX, elementIndexY, key))
			return true;
	}

	return false;
}

// Divide and conquer
bool Search3D(const int* values, int width, int height, int startX, int startY, int startZ, int endX, int endY, int endZ, int key)
{
	// If our search area has zero volume, bail out
	if (startX == endX || startY == endY || startZ == endZ)
		return false;

	int elementIndexX = (startX + endX) / 2;
	int elementIndexY = (startY + endY) / 2;
	int elementIndexZ = (startZ + endZ) / 2;
	int elementIndex = elementIndexZ * width * height + elementIndexY * width + elementIndexX;
	int element = values[elementIndex];

	if (element == key)
		return true;

	// if the element we looked at is less than the key, the key is not <= this location on x and y. remove that <= block
	if (element < key)
	{
		// search the back half
		if (Search3D(values, width, height, startX, startY, elementIndexZ+1, endX, endY, endZ, key))
			return true;

		// search the right half of the front half
		if (Search3D(values, width, height, elementIndexX + 1, startY, startZ, endX, endY, elementIndexZ + 1, key))
			return true;

		// search the lower half of the left half of the front half
		if (Search3D(values, width, height, startX, elementIndexY + 1, startZ, elementIndexX + 1, endY, elementIndexZ + 1, key))
			return true;
	}
	// else the element is greater than the key, so the key is not >= this location on x and y. remove that >= block.
	else
	{
		// search the front half
		if (Search3D(values, width, height, startX, startY, startZ, endX, endY, elementIndexZ, key))
			return true;

		// search the left half of the back half
		if (Search3D(values, width, height, startX, startY, elementIndexZ, elementIndexX, endY, endZ, key))
			return true;

		// search the upper half of the right half of the back half
		if (Search3D(values, width, height, elementIndexX, startY, elementIndexZ, endX, elementIndexY, endZ, key))
			return true;
	}

	return false;
}


std::mt19937 GetRNG()
{
#if DETERMINISTIC()
	return std::mt19937();
#else
	std::random_device rd;
	return std::mt19937(rd());
#endif
}

void DoTests1D()
{
	printf(__FUNCTION__ "()...\n");
	std::mt19937 rng = GetRNG();

	static const int c_numTests = 10000;
	std::uniform_int_distribution<int> numValuesDist(10, 1000);
	std::uniform_int_distribution<int> valueDist(0, 2000);

	std::vector<int> randomValues;

	int lastPercent = -1;
	for (int i = 0; i < c_numTests; ++i)
	{
		// print progress
		int percent = int(100.0f * float(i) / float(c_numTests - 1));
		if (lastPercent != percent)
		{
			lastPercent = percent;
			printf("\r%i%%", percent);
		}

		// make a random sized list of random values
		randomValues.resize(numValuesDist(rng));
		for (int& value : randomValues)
			value = valueDist(rng);

		// sort the list
		std::sort(randomValues.begin(), randomValues.end());

		// search with our algorithm
		int searchValue = valueDist(rng);
		bool found = Search1D(randomValues.data(), 0, (int)randomValues.size(), searchValue);

		// brute force search to verify
		bool foundBruteSearch = false;
		for (int value : randomValues)
		{
			if (value == searchValue)
			{
				foundBruteSearch = true;
				break;
			}
		}

		// report an error if the searches don't agree
		if (found != foundBruteSearch)
		{
			printf("\nERROR! brute force and divide and conquer disagree!!\n");
			return;
		}

	}
	printf("\r100%%\n");
}

std::vector<int> Transpose(const std::vector<int>& source, int width, int height)
{
	std::vector<int> ret(source.size());

	for (int iy = 0; iy < height; ++iy)
		for (int ix = 0; ix < width; ++ix)
			ret[ix * height + iy] = source[iy * width + ix];

	return ret;
}

template <size_t AXIS>
std::vector<int> Transpose(const std::vector<int>& source, int width, int height, int depth)
{
	std::vector<int> ret(source.size());

	auto coordToIndex = [](int x, int y, int z, int sizeX, int sizeY, int sizeZ)
	{
		return z * sizeX * sizeY + y * sizeX + x;
	};

	for (int iz = 0; iz < depth; ++iz)
	{
		for (int iy = 0; iy < height; ++iy)
		{
			for (int ix = 0; ix < width; ++ix)
			{
				int srcIndex = coordToIndex(ix, iy, iz, width, height, depth);
				int destIndex = 0;
				switch (AXIS)
				{
					// swap x axis with x axis - NO-OP.
					case 0: destIndex = coordToIndex(ix, iy, iz, width, height, depth); break;
					// swap x axis with y axis
					case 1: destIndex = coordToIndex(iy, ix, iz, height, width, depth); break;
					// swap x axis with z axis
					case 2: destIndex = coordToIndex(iz, iy, ix, depth, height, width); break;
				}
				ret[destIndex] = source[srcIndex];
			}
		}
	}

	return ret;
}

template <size_t AXIS>
void SortAxis(std::vector<int>& source, int width, int height)
{
	if (AXIS == 1)
	{
		source = Transpose(source, width, height);
		std::swap(width, height);
	}

	for (int iy = 0; iy < height; ++iy)
	{
		int* begin = &source[iy * width];
		int* end = &begin[width];
		std::sort(begin, end);
	}

	if (AXIS == 1)
	{
		source = Transpose(source, width, height);
		std::swap(width, height);
	}
}

template <size_t AXIS>
void SortAxis(std::vector<int>& source, int width, int height, int depth)
{
	if (AXIS == 1)
	{
		source = Transpose<AXIS>(source, width, height, depth);
		std::swap(width, height);
	}
	else if (AXIS == 2)
	{
		source = Transpose<AXIS>(source, width, height, depth);
		std::swap(width, depth);
	}

	for (int iy = 0; iy < height * depth; ++iy)
	{
		int* begin = &source[iy * width];
		int* end = &begin[width];
		std::sort(begin, end);
	}

	if (AXIS == 1)
	{
		source = Transpose<AXIS>(source, width, height, depth);
		std::swap(width, height);
	}
	else if (AXIS == 2)
	{
		source = Transpose<AXIS>(source, width, height, depth);
		std::swap(width, depth);
	}
}

void DoTests2D()
{
	printf(__FUNCTION__ "()...\n");
	std::mt19937 rng = GetRNG();

	static const int c_numTests = 10000;
	std::uniform_int_distribution<int> numValuesDist(5, 100);
	std::uniform_int_distribution<int> valueDist(0, 2000);

	std::vector<int> randomValues;

	int lastPercent = -1;
	for (int i = 0; i < c_numTests; ++i)
	{
		// print progress
		int percent = int(100.0f * float(i) / float(c_numTests - 1));
		if (lastPercent != percent)
		{
			lastPercent = percent;
			printf("\r%i%%", percent);
		}

		// make a random sized 2d array of random values
		int sizeX = numValuesDist(rng);
		int sizeY = numValuesDist(rng);
		randomValues.resize(sizeX * sizeY);
		for (int& value : randomValues)
			value = valueDist(rng);

		// Sort the rows, then sort the columns
		// At each location, the number above and left are <= in value.
		SortAxis<0>(randomValues, sizeX, sizeY);
		SortAxis<1>(randomValues, sizeX, sizeY);

		// search with our algorithm
		int searchValue = valueDist(rng);
		bool found = Search2D(randomValues.data(), sizeX, 0, 0, sizeX, sizeY, searchValue);

		// brute force search to verify
		bool foundBruteSearch = false;
		int bruteForceIndex = -1;
		for (int value : randomValues)
		{
			bruteForceIndex++;
			if (value == searchValue)
			{
				foundBruteSearch = true;
				break;
			}
		}

		// report an error if the searches don't agree
		if (found != foundBruteSearch)
		{
			printf("\nERROR! brute force and divide and conquer disagree on test %i!\n", i);
			return;
		}
	}
	printf("\r100%%\n");
}

void DoTests3D()
{
	printf(__FUNCTION__ "()...\n");
	std::mt19937 rng = GetRNG();

	static const int c_numTests = 10000;
	std::uniform_int_distribution<int> numValuesDist(5, 30);
	std::uniform_int_distribution<int> valueDist(0, 2000);

	std::vector<int> randomValues;

	int lastPercent = -1;
	for (int i = 0; i < c_numTests; ++i)
	{
		// print progress
		int percent = int(100.0f * float(i) / float(c_numTests - 1));
		if (lastPercent != percent)
		{
			lastPercent = percent;
			printf("\r%i%%", percent);
		}

		// make a random sized 3d array of random values
		int sizeX = numValuesDist(rng);
		int sizeY = numValuesDist(rng);
		int sizeZ = numValuesDist(rng);

		randomValues.resize(sizeX * sizeY * sizeZ);
		for (int& value : randomValues)
			value = valueDist(rng);

		// Sort each dimension
		// At each location, the number above and left are <= in value.
		SortAxis<0>(randomValues, sizeX, sizeY, sizeZ);
		SortAxis<1>(randomValues, sizeX, sizeY, sizeZ);
		SortAxis<2>(randomValues, sizeX, sizeY, sizeZ);

		// search with our algorithm
		int searchValue = valueDist(rng);
		bool found = Search3D(randomValues.data(), sizeX, sizeY, 0, 0, 0, sizeX, sizeY, sizeZ, searchValue);

		// brute force search to verify
		bool foundBruteSearch = false;
		int bruteForceIndex = -1;
		for (int value : randomValues)
		{
			bruteForceIndex++;
			if (value == searchValue)
			{
				foundBruteSearch = true;
				break;
			}
		}

		// report an error if the searches don't agree
		if (found != foundBruteSearch)
		{
			printf("\nERROR! brute force and divide and conquer disagree on test %i!\n", i);
			return;
		}
	}
	printf("\r100%%\n");
}

int main(int argc, char** argv)
{
	DoTests1D();
	DoTests2D();
	DoTests3D();
	return 0;
}
