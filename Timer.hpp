// ================================================ //
// File: Timer.hpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Defines Timer class.
// ================================================ //

#include "stdafx.hpp"

// ================================================ //

#ifndef __TIMER_HPP__
#define __TIMER_HPP__

// ================================================ //

// A timer that starts from zero. Uses milliseconds.
class Timer
{
public:
	// Empty constructor.
	explicit Timer(const bool start = false);

	// Empty destructor.
	~Timer(void);

	// Starts the timer, erasing any previous time.
	const Uint restart(void);

	// Stops the timer.
	void stop(void);

	// Pauses the timer.
	void pause(void);

	// Unpauses the timer.
	void unpause(void);

	// Get the time in milliseconds since the timer was started.
	const Uint getTicks(void);

	// Getters

	// Returns true if the timer is active.
	const bool isStarted(void) const;

	// Returns true if the timer is paused.
	const bool isPaused(void) const;

	// --- //

	// Delays the calling thread in ms.
	static void Delay(const Uint ms);

	// A multiplier applied to speed up the simulation.
	// e.g., set to 10 to run the simulation at 10x speed.
	static Uint Multiplier;

private:
	Uint m_startTicks;
	Uint m_pausedTicks;
	bool m_paused;
	bool m_started;
};

// ================================================ //

// Getters

inline const bool Timer::isStarted(void) const{
	return m_started;
}

inline const bool Timer::isPaused(void) const{
	return m_paused;
}

// ================================================ //

#endif

// ================================================ //