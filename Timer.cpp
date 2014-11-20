// ================================================ //
// File: Timer.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Implements Timer class.
// ================================================ //

#include "Timer.hpp"

// ================================================ //

Uint Timer::Multiplier = 1;

// ================================================ //

Timer::Timer(const bool start) :
m_startTicks(0),
m_pausedTicks(0),
m_paused(false),
m_started(false)
{
	if (start){
		this->restart();
	}
}

// ================================================ //

Timer::~Timer(void)
{

}

// ================================================ //

Uint Timer::restart(void)
{
	m_started = true;
	m_paused = false;

	m_startTicks = GetTickCount() * Timer::Multiplier;
	return m_startTicks;
}

// ================================================ //

void Timer::stop(void)
{
	m_started = false;
	m_paused = false;
}

// ================================================ //

void Timer::pause(void)
{
	if (m_started == true && m_paused == false){
		m_paused = true;

		m_pausedTicks = GetTickCount() * Timer::Multiplier - m_startTicks;
	}
}

// ================================================ //

void Timer::unpause(void)
{
	if (m_paused == true){
		m_paused = false;

		m_startTicks = GetTickCount() * Timer::Multiplier - m_pausedTicks;

		m_pausedTicks = 0;
	}
}

// ================================================ //

Uint Timer::getTicks(void)
{
	if (m_started == true){
		return (m_paused == true) ? m_pausedTicks :
			static_cast<Uint>(GetTickCount() * Timer::Multiplier - m_startTicks);
	}

	return 0;
}

// ================================================ //

void Timer::Delay(const Uint ms)
{
	Sleep(ms / Timer::Multiplier);
}

// ================================================ //