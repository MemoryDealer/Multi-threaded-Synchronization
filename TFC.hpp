// ================================================ //
// File: TFC.hpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Defines TFC (Task Force Command) class.
// ================================================ //

#ifndef __TFC_HPP__
#define __TFC_HPP__

// ================================================ //

#include "Asteroid.hpp"
#include "Probe.hpp"
#include "Net.hpp"
#include "Timer.hpp"

// ================================================ //

struct ProbeRecord{	
	IPAddress ip;
	Uint id;
	Uint type;
	Uint state;
};

// ================================================ //

class TFC
{
public:
	// Initializes member variables and calls init().
	explicit TFC(void);

	// Closes socket.
	~TFC(void);

	// Sets up server socket, binds and begins listening.
	// Returns zero on success, otherwise the error code is returned.
	int init(void);

	// Accept incoming probe connections and register them.
	void launchProbes(void);
	
	// Creates a thread for navigating the asteroid field and returns. This
	// must return so the GUI can be updated.
	void enterAsteroidField(void);

	// Accept requests from probes and process them.
	int navigateAsteroidField(void);

	// Getters

	// Returns number of probes launched.
	const int getNumProbes(void) const;

	// Returns current time.
	const Uint getCurrentTime(void) const;

	// Setters

	// --- //

	static const std::string Port;

private:
	AsteroidContainer m_asteroids;
	std::vector<ProbeRecord> m_probes;
	SOCKET m_socket;
	bool m_fleetAlive, m_inAsteroidField;
	int m_shields;
	int m_asteroidsDestroyed;
	std::shared_ptr<Timer> m_pTimer;
};

// ================================================ //

// Getters

inline const int TFC::getNumProbes(void) const{
	return m_probes.size();
}

inline const Uint TFC::getCurrentTime(void) const{
	return m_pTimer->getTicks();
}

// Setters


// ================================================ //

#endif

// ================================================ //