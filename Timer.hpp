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
	Uint restart(void);

	// Stops the timer.
	void stop(void);

	// Pauses the timer.
	void pause(void);

	// Unpauses the timer.
	void unpause(void);

	// Get the time in milliseconds since the timer was started.
	Uint getTicks(void);

	// Getters

	// Returns true if the timer is active.
	bool isStarted(void) const;

	// Returns true if the timer is paused.
	bool isPaused(void) const;

	// Setters

	// Manually set the start ticks value (determined by GetTickCount() when started).
	void setStartTicks(const int ticks);

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

inline bool Timer::isStarted(void) const{
	return m_started;
}

inline bool Timer::isPaused(void) const{
	return m_paused;
}

// Setters

inline void Timer::setStartTicks(const int ticks){
	m_startTicks = ticks;
}

// ================================================ //

#endif

// ================================================ //