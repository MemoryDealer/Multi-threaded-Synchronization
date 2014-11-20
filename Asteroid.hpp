// ================================================ //
// File: Asteroid.hpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Defines AsteroidData container class and asteroid
// record data.
// ================================================ //

#ifndef __ASTEROID_HPP__
#define __ASTEROID_HPP__

// ================================================ //

#include "stdafx.hpp"

// ================================================ //

// Data structure to describe an asteroid.
struct Asteroid{
	Uint id;
	Uint mass;
	Uint discoveryTime;
	Uint impactTime;
};

// ================================================ //
// A container with stack operations.
class AsteroidContainer
{
public:
	explicit AsteroidContainer(void);
	~AsteroidContainer(void);

	// Pushes an object into the stack. Returns false if stack is full.
	bool insert(const Asteroid& asteroid);

	// Pops the top item off the stack.
	const Asteroid remove(void);

	// Returns true if stack is empty.
	const bool empty(void) const;

	// Returns true if stack is full.
	const bool full(void) const;

	// --- //

	// Maximum number of items in container.
	static const int MAX = 15;

private:
	// Array-based stack.
	Asteroid m_data[MAX];
	int top;
};

// ================================================ //

inline const bool AsteroidContainer::empty(void) const{
	return (top == -1);
}

inline const bool AsteroidContainer::full(void) const{
	return (top == MAX - 1);
}

// ================================================ //
// B - Queue
// ================================================ //

// ================================================ //
// A container with queue operations.
//class AsteroidContainer
//{
//public:
//	explicit AsteroidContainer(void);
//	~AsteroidContainer(void);
//
//	// Pushes an object into the stack. Returns false if stack is full.
//	bool insert(const Asteroid& asteroid);
//
//	// Pops the top item off the stack.
//	const Asteroid remove(void);
//
//	// Returns true if stack is empty.
//	const bool empty(void) const;
//
//	// Returns true if stack is full.
//	const bool full(void) const;
//
//	// --- //
//
//	// Maximum number of items in container.
//	static const int MAX = 15;
//
//private:
//	// Array-based stack.
//	Asteroid m_data[MAX];
//	int m_size;
//	int m_front, m_rear;
//};
//
//// ================================================ //
//
//inline const bool AsteroidContainer::empty(void) const{
//	return (m_size == 0);
//}
//
//inline const bool AsteroidContainer::full(void) const{
//	return (m_size == AsteroidContainer::MAX - 1);
//}

// ================================================ //



// ================================================ //

#endif

// ================================================ //