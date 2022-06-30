#pragma once
#include <charconv>
#include <cstring>
#include <iomanip>
#include <iostream>

template< class T >
class ReversedVector
{
	T* data;
	size_t size;
	size_t capacity;

	bool checkCapacity(size_t newSize);

  public:
	ReversedVector();
	explicit ReversedVector(size_t len);
	ReversedVector(size_t len, T init);
	ReversedVector(std::nullptr_t pVoid);

	void resize(size_t newSize);
	size_t getSize() const { return size; }

	void push_front(T elem);
	void push_back(T elem);

	ReversedVector& operator=(const ReversedVector& other);
	T& operator[](size_t ind) const { return data[ind]; }

	template< class V >
	friend std::ostream& operator<<(std::ostream& out, const ReversedVector< V >& arg);

	~ReversedVector() { delete[] data; }
};

template< class T >
ReversedVector< T >::ReversedVector()
{
	data = nullptr;
	size = 0;
	capacity = 0;
}

template< class T >
ReversedVector< T >::ReversedVector(size_t len)
{
	size = len;
	data = new T[len];
	capacity = len;
}

template< class T >
ReversedVector< T >::ReversedVector(size_t len, T init) : ReversedVector(len)
{
	for (size_t i = 0; i < len; i++)
		data[i] = init;
}

template< class T >
ReversedVector< T >::ReversedVector(std::nullptr_t pVoid)
{
	data = pVoid;
	size = 0;
	capacity = 0;
}

template< class T >
bool ReversedVector< T >::checkCapacity(size_t newSize)
{
	return capacity >= newSize;
}

template< class T >
void ReversedVector< T >::resize(size_t newSize)
{
	if (newSize == 0)
	{
		delete[] data;
		data = new T[1];
		size = 1;
		return;
	}
	if (checkCapacity(newSize))
	{
		size = newSize;
		return;
	}
	if (capacity == 0)
		capacity = 1;
	while (capacity < newSize)
		capacity *= 2;
	T* newVec = new T[capacity];
	std::copy(data, data + std::min(newSize, size), newVec);
	delete[] data;
	size = newSize;
	data = newVec;
}

template< class T >
void ReversedVector< T >::push_front(T elem)
{
	if (capacity < size + 1)
		capacity = size + 1;
	T* newVec = new T[capacity];
	newVec[0] = elem;
	std::copy(data, data + size, newVec + 1);
	delete[] data;
	data = newVec;
	size++;
}

template< class T >
void ReversedVector< T >::push_back(T elem)
{
	resize(size + 1);
	data[size - 1] = elem;
}

template< class T >
ReversedVector< T >& ReversedVector< T >::operator=(const ReversedVector< T >& other)
{
	if (this == &other)
		return *this;
	T* newVec = new T[other.capacity];
	std::copy(other.data, other.data + other.size, newVec);
	delete[] data;
	data = newVec;
	size = other.size;
	capacity = other.capacity;
	return *this;
}

template< class V >
std::ostream& operator<<(std::ostream& out, const ReversedVector< V >& arg)
{
	if (arg.size < 2)
		return (arg.size == 0 ? out : out << arg[arg.size - 1]);
	out << arg[arg.size - 1];
	size_t prev = arg.size - 2;
	for (size_t i = prev; i <= prev; i--)
	{
		char oldFiller = out.fill('0');
		out << std::setw(9) << arg[i];
		out.fill(oldFiller);
		prev = i;
	}
	return out;
}
