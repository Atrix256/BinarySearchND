#include <stdio.h>
#include <random>
#include <vector>
#include <algorithm>

#define DETERMINISTIC() false

// Just binary search
bool Search1D(const int* values, size_t count, int key)
{
	if (count == 0)
		return false;

	size_t elementIndex = count / 2;
	int element = values[elementIndex];

	if (element == key)
		return true;

	if (element < key)
		return Search1D(&values[elementIndex + 1], count - (elementIndex + 1), key);
	else
		return Search1D(values, elementIndex, key);
}

// Divide and conquer
bool Search2D(const int* values, size_t width, size_t height, size_t startX, size_t startY, size_t endX, size_t endY, int key)
{
	// If our search area has zero volume, bail out
	if (startX == endX || startY == endY)
		return false;

	size_t elementIndexX = (startX + endX) / 2;
	size_t elementIndexY = (startY + endY) / 2;
	size_t elementIndex = elementIndexY * width + elementIndexX;

	int element = values[elementIndex];

	if (element == key)
		return true;

	// if the element we looked at is less than the key, the key is not <= this location on x and y. remove that <= block
	if (element < key)
	{
		// the right side
		if (Search2D(values, width, height, elementIndexX + 1,            startY,              endX, endY, key))
			return true;

		// the bottom side
		if (Search2D(values, width, height,            startX, elementIndexY + 1, elementIndexX + 1, endY, key))
			return true;
	}
	// else the element is greater than the key, so the key is not >= this location on x and y. remove that >= block.
	else
	{
		// the left side
		if (Search2D(values, width, height,        startX, startY, elementIndexX,          endY, key))
			return true;

		// the top side
		if (Search2D(values, width, height, elementIndexX, startY,          endX, elementIndexY, key))
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
		bool found = Search1D(randomValues.data(), randomValues.size(), searchValue);

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

std::vector<int> Transpose(const std::vector<int>& source, size_t width, size_t height)
{
	std::vector<int> ret(source.size());

	for (size_t iy = 0; iy < height; ++iy)
		for (size_t ix = 0; ix < width; ++ix)
			ret[ix * height + iy] = source[iy * width + ix];

	return ret;
}

void SortRows(std::vector<int>& source, size_t width, size_t height)
{
	for (size_t iy = 0; iy < height; ++iy)
	{
		int* begin = &source[iy * width];
		int* end = &begin[width];
		std::sort(begin, end);
	}
}

void SortCols(std::vector<int>& source, size_t width, size_t height)
{
	source = Transpose(source, width, height);
	SortRows(source, height, width);
	source = Transpose(source, height, width);
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
		size_t sizeX = numValuesDist(rng);
		size_t sizeY = numValuesDist(rng);
		randomValues.resize(sizeX * sizeY);
		for (int& value : randomValues)
			value = valueDist(rng);

		// Sort the rows, then sort the columns
		// At each location, the number above and left are <= in value.
		SortRows(randomValues, sizeX, sizeY);
		SortCols(randomValues, sizeX, sizeY);

		// search with our algorithm
		int searchValue = valueDist(rng);
		bool found = Search2D(randomValues.data(), sizeX, sizeY, 0, 0, sizeX, sizeY, searchValue);

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


int main(int argc, char** argv)
{
	DoTests1D();
	DoTests2D();
	return 0;
}

/*
TODO:
* Do 2D at least.
* 3d as well?
? should we make a graph of how many searches were done for different sizes?

NOTES:
* coding recursively cause its easier and easier to understand.
 * for 1d, it could be a loop.
 * for 2d and higher, could have a stack of sections to search that you add to and pop from, and continue til it's empty.
 * 3D and higher leaving for you!

*/