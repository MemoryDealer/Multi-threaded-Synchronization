// ================================================ //
// File: Semaphore.hpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Defines Semaphore class.
// ================================================ //

#include "stdafx.hpp"

// ================================================ //
// A semaphore object using C++11 features.
class Semaphore
{
public:
	// Initializes member variables.
	Semaphore(const Uint count = 0);

	// Empty destructor.
	~Semaphore(void);

	// Decrements count, if negative, calling process blocks.
	void wait(void);

	// Increments count, allows next blocking process in.
	void signal(void);

private:
	Uint m_count;
	std::mutex m_mutex;
	std::condition_variable m_cr;
};

// ================================================ //