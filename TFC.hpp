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

// ================================================ //

class TFC
{
public:
	explicit TFC(const unsigned int numPhaserProbes);
	~TFC(void);

	// Sets up server socket, binds and begins listening.
	// Returns zero on success, otherwise the error code is returned.
	int init(void);

	// Spawns a thread with function launchProbes().
	void initProbeLaunching(void);

	// Accept incoming probe connections and register them.
	int launchProbes(void);

	// Accept requests from probes and process them.
	int navigateAsteroidField(void);

	// Returns number of probes launched.
	const int getNumProbes(void) const;

	// --- //

	static const std::string Port;
private:
	AsteroidContainer m_asteroids;
	std::vector<Probe> m_probes;
	SOCKET m_socket;
	bool m_fleetAlive, m_exitedAsteroidField;
};

// ================================================ //

inline const int TFC::getNumProbes(void) const{
	return m_probes.size();
}

// ================================================ //

#endif

// ================================================ //