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
	if (this->empty()){
		return Asteroid();
	}

	return m_data[top--];
}

// ================================================ //

//AsteroidContainer::AsteroidContainer(void) :
//m_data(),
//m_size(0),
//m_front(0),
//m_rear(AsteroidContainer::MAX - 1)
//{
//	//std::fill(m_data[0], m_data[MAX], nullptr);
//}
//
//// ================================================ //
//
//AsteroidContainer::~AsteroidContainer(void)
//{
//
//}
//
//// ================================================ //
//
//bool AsteroidContainer::insert(const Asteroid& asteroid)
//{
//	if (this->full()){
//		return false;
//	}
//
//	m_rear = (m_rear + 1) % AsteroidContainer::MAX;
//	m_data[m_rear] = asteroid;
//	++m_size;
//	return true;
//}
//
//// ================================================ //
//
//const Asteroid AsteroidContainer::remove(void)
//{
//	if (this->empty()){
//		return Asteroid();
//	}
//
//	Asteroid front = m_data[m_front];
//
//	m_front = (m_front + 1) % AsteroidContainer::MAX;
//	--m_size;
//	return front;
//}

// ================================================ //