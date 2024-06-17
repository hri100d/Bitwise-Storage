#include "MultiSet.h"
#include <fstream>

void MultiSet::free()
{
	delete[] _container;
}
void MultiSet::copy(const MultiSet& other)
{
	size_t size = getBucketCount(other._maxNumber, other._maxBits);
	_container = new uint8_t[size]{ 0 };
	for (size_t i = 0; i < size; ++i)
	{
		_container[i] |= other._container[i];
	}
	_maxNumber = other._maxNumber;
	_maxBits = other._maxBits;
}

unsigned getBucketCount(unsigned maxNumber, const unsigned maxBits)
{
	++maxNumber;
	if ((maxNumber * maxBits) % 8)
	{
		return ((maxNumber * maxBits) / 8) + 1;
	}

	return (maxNumber * maxBits) / 8;
}

unsigned MultiSet::getBucketIndex(const unsigned number) const
{
	return number * _maxBits / 8;
}
void MultiSet::oneUp(unsigned& bucketIndex, unsigned& bitIndex) const
{
	++bitIndex;
	for (unsigned i = 0; i < _maxBits; ++i)
	{
		if (bitIndex == 8)
		{
			--bucketIndex;
			bitIndex = 0;
		}

		if ((_container[bucketIndex] & (1 << bitIndex)) == 0)
		{
			_container[bucketIndex] |= (1 << bitIndex);
			break;
		}
		else
		{
			_container[bucketIndex] &= ~(1 << bitIndex);
			++bitIndex;
		}
	}
}
void MultiSet::changeCount(unsigned number, const unsigned count) const
{
	++number;
	unsigned bucketIndex = getBucketIndex(number);
	unsigned bitIndex = 7 - (number * _maxBits % 8) + 1;

	for (unsigned i = 0; i < _maxBits; ++i)
	{
		if (bitIndex == 8)
		{
			--bucketIndex;
			bitIndex = 0;
		}

		if ((count & (1 << i)) != 0)
		{
			_container[bucketIndex] |= (1 << bitIndex);
		}

		++bitIndex;
	}
}
unsigned MultiSet::countOfTheNumber(unsigned& bucketIndex, unsigned& bitIndex) const
{
	uint8_t count = 0;

	for (unsigned i = 0; i < _maxBits; ++i)
	{
		if (bitIndex == -1)
		{
			bitIndex = 7;
			++bucketIndex;
		}

		if ((_container[bucketIndex] & (1 << bitIndex)) != 0)
		{
			count |= 1 << _maxBits - (i + 1);
		}

		--bitIndex;
	}

	return count;
}

MultiSet::MultiSet(const unsigned maxNumber, const unsigned maxBits)
{
	if (maxBits <= 8 && maxBits >= 1)
	{
		_maxBits = maxBits;
	}
	else
	{
		_maxBits = 1;
	}

	_maxNumber = maxNumber;
	_container = new uint8_t[getBucketCount(_maxNumber, _maxBits)]{ 0 };
	
}
MultiSet::MultiSet(const MultiSet& other)
{
	copy(other);
}
MultiSet& MultiSet::operator=(const MultiSet& other)
{
	if (this != &other)
	{
		free();
		copy(other);
	}

	return *this;
}
MultiSet::~MultiSet()
{
	free();
}

void MultiSet::addNumber(const unsigned number)
{
	if (number > _maxNumber)
		return;

	unsigned bucketIndex = getBucketIndex(number);
	unsigned bitIndex = 7 - (number * _maxBits % 8);
	size_t count = countOfTheNumber(bucketIndex, bitIndex);

	if (count == pow(2, _maxBits) - 1)
	{
		std::cout << "You cant add this number anymore.\n";
		return;
	}

	oneUp(bucketIndex, bitIndex);
}
unsigned MultiSet::countOfTheNumber(const unsigned number) const
{
	unsigned bucketIndex = getBucketIndex(number);
	unsigned bitIndex = 7 - (number * _maxBits % 8);

	uint8_t count = 0;
	for (unsigned i = 0; i < _maxBits; ++i)
	{
		
		if (bitIndex == -1)
		{
			bitIndex = 7;
			++bucketIndex;
		}
		if ((_container[bucketIndex] & (1 << bitIndex)) != 0)
		{
			count |= 1 << _maxBits - (1 + i);
		}
		--bitIndex;
	}

	return count;
}
void MultiSet::print() const
{
	std::cout << "{ ";
	for (unsigned i = 0; i <= _maxNumber; ++i)
	{
		size_t count = countOfTheNumber(i);

		if (count > 0)
		{
			std::cout << i << "[" << count << "]" << " ";
		}
	}
	std::cout << "}\n";
}
void MultiSet::printBits() const
{
	for (unsigned i = 0; i < getBucketCount(_maxNumber, _maxBits); ++i)
	{
		std::cout << "{ ";

		for (int j = 7; j > -1; --j)
		{
			if ((_container[i] & (1 << j)) != 0)
			{
				std::cout << 1;
			}
			else
			{
				std::cout << 0;
			}
		}

		std::cout << " }";
	}

	std::cout << "\n";
}
void MultiSet::serialization(const char* fileName) const
{
	std::ofstream out(fileName, std::ios::binary);
	if (!out.is_open())
	{
		std::cout << "Error\n";
		return;
	}

	out.write((const char*)&_maxNumber, sizeof(_maxNumber));
	out.write((const char*)&_maxBits, sizeof(_maxBits));
	out.write((const char*)_container, sizeof(uint8_t) * getBucketCount(_maxNumber, _maxBits));
}
void MultiSet::deserialization(const char* fileName)
{
	std::ifstream in(fileName, std::ios::binary);
	if (!in.is_open())
	{
		std::cout << "Error\n";
		return;
	}

	in.read((char*)&_maxNumber, sizeof(_maxNumber));
	in.read((char*)&_maxBits, sizeof(_maxBits));
	size_t size = getBucketCount(_maxNumber, _maxBits);

	free();
	_container = new uint8_t[size];
	in.read((char*)_container, sizeof(uint8_t) * size);
}
MultiSet section(const MultiSet& lhs, const MultiSet& rhs)
{
	unsigned maxBits = 0;
	unsigned maxNumber = 0;
	if (lhs._maxBits >= rhs._maxBits)
	{
		maxBits = rhs._maxBits;
	}
	else
	{
		maxBits = lhs._maxBits;
	}

	if (lhs._maxNumber <= rhs._maxNumber)
	{
		maxNumber = lhs._maxNumber;
	}
	else
	{
		maxNumber = rhs._maxNumber;
	}

	MultiSet temp(maxNumber, maxBits);

	for (unsigned i = 0; i <= maxNumber; ++i)
	{
		unsigned lhsCount = lhs.countOfTheNumber(i);
		unsigned rhsCount = rhs.countOfTheNumber(i);
		if (lhsCount <= rhsCount)
		{
			temp.changeCount(i, lhsCount);
		}
		else
		{
			temp.changeCount(i, rhsCount);
		}
	}

	return temp;
}
MultiSet difference(const MultiSet& lhs, const MultiSet& rhs)
{
	unsigned maxBitsUpperBound = 0;
	unsigned maxNumberUpperBound = 0;
	unsigned maxNumberLowerBound = 0;
	unsigned maxBitsLowerBound = 0;

	if (lhs._maxBits >= rhs._maxBits)
	{
		maxBitsUpperBound = lhs._maxBits;
		maxBitsLowerBound = rhs._maxBits;
	}
	else
	{
		maxBitsUpperBound = rhs._maxBits;
		maxBitsLowerBound = lhs._maxBits;
	}

	if (lhs._maxNumber >= rhs._maxNumber)
	{
		maxNumberUpperBound = lhs._maxNumber;
		maxNumberLowerBound = rhs._maxNumber;
	}
	else
	{
		maxNumberUpperBound = rhs._maxNumber;
		maxNumberLowerBound = lhs._maxNumber;
	}

	MultiSet temp(maxNumberUpperBound, maxBitsUpperBound);

	for (unsigned i = 0; i < maxNumberLowerBound; ++i)
	{
		unsigned lhsCount = lhs.countOfTheNumber(i);
		unsigned rhsCount = rhs.countOfTheNumber(i);

		if (lhsCount <= rhsCount)
		{
			temp.changeCount(i, rhsCount - lhsCount);
		}
		else
		{
			temp.changeCount(i, lhsCount - rhsCount);
		}
	}

	for (unsigned i = maxNumberLowerBound; i <= maxNumberUpperBound; ++i)
	{
		if (lhs._maxNumber > rhs._maxNumber)
		{
			temp.changeCount(i, lhs.countOfTheNumber(i));
		}
		else
		{
			temp.changeCount(i, rhs.countOfTheNumber(i));
		}
		
	}

	return temp;
}
MultiSet complement(const MultiSet& multiSet)
{
	MultiSet temp(multiSet._maxNumber, multiSet._maxBits);
	for (unsigned i = 0; i < getBucketCount(multiSet._maxNumber, multiSet._maxBits); ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			temp._container[i] |= ~multiSet._container[i];
		}
	}

	return temp;
}
