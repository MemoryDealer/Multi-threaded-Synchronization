// ================================================ //
// File: Semaphore.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Implements Semaphore class.
// ================================================ //

#include "Semaphore.hpp"

// ================================================ //

Semaphore::Semaphore(const Uint count) :
m_count(count),
m_mutex(),
m_cr()
{

}

// ================================================ //

Semaphore::~Semaphore(void)
{
	
}

// ================================================ //

void Semaphore::wait(void)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	while (m_count == 0){
		m_cr.wait(lock);
	}
	--m_count;
}

// ================================================ //

void Semaphore::signal(void)
{	
	std::unique_lock<std::mutex> lock(m_mutex);
	++m_count;
	m_cr.notify_one();
}

// ================================================ //