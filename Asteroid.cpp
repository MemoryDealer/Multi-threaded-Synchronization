// ================================================ //
// File: Asteroid.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Implements AsteroidData container class and asteroid
// record data.
// ================================================ //

#include "Asteroid.hpp"

// ================================================ //

AsteroidContainer::AsteroidContainer(void) :
m_data(),
top(-1)
{

}

// ================================================ //

AsteroidContainer::~AsteroidContainer(void)
{

}

// ================================================ //

bool AsteroidContainer::insert(const Asteroid& asteroid)
{
	if (top < MAX - 1){
		m_data[++top] = asteroid;
		return true;
	}

	return false;
}

// ================================================ //

const Asteroid AsteroidContainer::remove(void)
{
	return m_data[top--];
}

// ================================================ //