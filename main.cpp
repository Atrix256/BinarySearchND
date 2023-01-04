#include <stdio.h>
#include <random>
#include <vector>
#include <algorithm>

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
		return Search1D(values, startX + 1, endX, key);
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

void SortRows(std::vector<int>& source, int width, int height)
{
	for (int iy = 0; iy < height; ++iy)
	{
		int* begin = &source[iy * width];
		int* end = &begin[width];
		std::sort(begin, end);
	}
}

void SortCols(std::vector<int>& source, int width, int height)
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
		int sizeX = numValuesDist(rng);
		int sizeY = numValuesDist(rng);
		randomValues.resize(sizeX * sizeY);
		for (int& value : randomValues)
			value = valueDist(rng);

		// Sort the rows, then sort the columns
		// At each location, the number above and left are <= in value.
		SortRows(randomValues, sizeX, sizeY);
		SortCols(randomValues, sizeX, sizeY);

		// search with our algorithm
		int searchValue = valueDist(rng);
		bool found = Search2D(randomValues.data(), sizeX, 0, 0, sizeX, sizeY, searchValue);

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
* 3d as well!
? should we make a graph of how many searches were done for different sizes? yep. how do we do 2d and 3d though?

NOTES:
* coding recursively cause its easier and easier to understand.
 * for 1d, it could be a loop.
 * for 2d and higher, could have a stack of sections to search that you add to and pop from, and continue til it's empty.
 * 3D and higher leaving for you!

*/