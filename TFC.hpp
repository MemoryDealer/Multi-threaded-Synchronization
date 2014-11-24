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
#include "Timer.hpp"
#include "Semaphore.hpp"

// ================================================ //

// Record for storing probe data on TFC.
struct ProbeRecord{
	SOCKET socket;
	Uint id;
	Uint type;
};

// Any event to be processed by the GUI update thread.
struct GUIEvent{
	Uint type;
	int id;
	union{
		Asteroid asteroid;
		int x;
	};
};

enum GUIEventType{
	NONE = 0,
	ASTEROID_FOUND,
	ASTEROID_REMOVED,
	ASTEROID_DESTROYED,
	ASTEROID_COLLISION,
	PROBE_TERMINATED,
	FLEET_DESTROYED,
	FLEET_SURVIVED
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

	// Sets the local flag m_inAsteroidField to true.
	void enterAsteroidField(void);

	// Accept launch requests from probes and process them.
	void launchProbes(void);

	// Process requests from a single probe.
	void updateProbe(const ProbeRecord& probe);

	// Getters

	// Returns number of probes launched.
	const int getNumProbes(void) const;

	// Returns current time.
	const Uint getCurrentTime(void) const;

	// Returns next GUIEvent in queue or nullptr if empty.
	const GUIEvent getNextGUIEvent(void);

	// Returns true if there are GUI events in the queue.
	const bool hasGUIEvent(void) const;

	// Returns shield level.
	const Uint getShields(void) const;

	// Returns number of asteroids destroyed.
	const Uint getNumAsteroidsDestroyed(void) const;

	// Returns number of asteroids still in asteroid field.
	const Uint getNumAsteroidsAwaitingDestruction(void) const;

	// Returns true if fleet is currently in asteroid field.
	const bool isInAsteroidField(void) const;

	// Returns number of phaser probes launched before navigating field.
	const Uint getNumPhaserProbesLaunched(void) const;

	// --- //

	// Port the TFC listens on.
	static const std::string Port;

private:
	AsteroidContainer m_asteroids;
	// Semaphores for synchronized access to AsteroidContainer.
	Semaphore m_mutex, m_empty, m_full;
	// List of all probes that have been launched.
	std::vector<ProbeRecord> m_probes;
	SOCKET m_socket;
	bool m_fleetAlive, m_inAsteroidField;
	int m_shields;
	Uint m_asteroidsDestroyed;
	std::shared_ptr<Timer> m_pClock;
	std::queue<GUIEvent> m_guiEvents;
	bool m_scoutActive;
	Uint m_numPhaserProbesLaunched;
};

// ================================================ //

inline void TFC::enterAsteroidField(void){
	m_inAsteroidField = true;
	m_pClock->restart();
}

// Getters

inline const int TFC::getNumProbes(void) const{
	return m_probes.size();
}

inline const Uint TFC::getCurrentTime(void) const{
	return m_pClock->getTicks();
}

inline const GUIEvent TFC::getNextGUIEvent(void){
	GUIEvent next;
	ZeroMemory(&next, sizeof(next));
	if (m_guiEvents.empty()){
		next.type = GUIEventType::NONE;
	}
	else{
		next = m_guiEvents.front();
		m_guiEvents.pop();
	}

	return next;
}

inline const bool TFC::hasGUIEvent(void) const{
	return (m_guiEvents.size() > 0);
}

inline const Uint TFC::getShields(void) const{
	return m_shields;
}

inline const Uint TFC::getNumAsteroidsDestroyed(void) const{
	return m_asteroidsDestroyed;
}

inline const Uint TFC::getNumAsteroidsAwaitingDestruction(void) const{
	return (m_asteroidsDestroyed <= 55) ? 55 - m_asteroidsDestroyed :
		0;
}

inline const bool TFC::isInAsteroidField(void) const{
	return m_inAsteroidField;
}

inline const Uint TFC::getNumPhaserProbesLaunched(void) const{
	return m_numPhaserProbesLaunched;
}

// ================================================ //

#endif

// ================================================ //