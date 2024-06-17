#pragma once
#include <iostream>

class MultiSet
{
private:
	unsigned _maxNumber;
	unsigned _maxBits;
	uint8_t* _container;

	void free();
	void copy(const MultiSet& other);

	unsigned getBucketIndex(const unsigned maxNumber) const;
	void oneUp(unsigned& bucketIndex, unsigned& bitIndex) const;
	void changeCount(unsigned number, const unsigned count) const;
	unsigned countOfTheNumber(unsigned& bucketIndex, unsigned& bitIndex) const;

public:

	MultiSet() = delete;
	MultiSet(const unsigned maxNumber, const unsigned maxBits);
	MultiSet(const MultiSet& other);
	MultiSet& operator=(const MultiSet& other);
	~MultiSet();

	void addNumber(const unsigned number);
	unsigned countOfTheNumber(const unsigned number) const;
	void print() const;
	void printBits() const;
	void serialization(const char* fileName) const;
	void deserialization(const char* fileName);
	friend MultiSet section(const MultiSet& lhs, const MultiSet& rhs);
	friend MultiSet difference(const MultiSet& lhs, const MultiSet& rhs);
	friend MultiSet complement(const MultiSet& multiSet);
};

unsigned getBucketCount(unsigned maxNumber, const unsigned maxBits);
