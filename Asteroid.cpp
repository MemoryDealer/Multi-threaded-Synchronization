// ================================================ //
// File: Asteroid.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Implements AsteroidData container class.
// ================================================ //

#include "Asteroid.hpp"

// ================================================ //

AsteroidContainer::AsteroidContainer(void) :
m_data(),
m_size(0)
{

}

// ================================================ //

AsteroidContainer::~AsteroidContainer(void)
{

}

// ================================================ //

bool AsteroidContainer::insert(const Asteroid& asteroid)
{
	if (this->full()){
		return false;
	}

	// Shift all items of higher priority right.
	int i;
	for (i = m_size - 1; i >= 0; --i){
		if (asteroid.impactTime > m_data[i].impactTime){
			m_data[i + 1] = m_data[i];
		}
		else{
			break;
		}
	}

	// Insert the asteroid data.
	m_data[i + 1] = asteroid;
	++m_size;

	return true;
}

// ================================================ //

const Asteroid AsteroidContainer::remove(void)
{
	if (this->empty()){
		// Safety measure.
		return Asteroid();
	}

	// Return the last asteroid in queue (highest priority).
	return m_data[--m_size];
}

// ================================================ //