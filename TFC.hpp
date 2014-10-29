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

struct GUIEvent{
	Uint type;
	std::string str;
};

enum GUIEventType{
	NONE = 0,
	NEW_UPDATE,
	NEW_ASTEROID,
	SHIELDS_HIT
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

	// Accept requests from probes and process them.
	void update(void);

	// Getters

	// Returns number of probes launched.
	const int getNumProbes(void) const;

	// Returns current time.
	const Uint getCurrentTime(void) const;

	// Returns next GUIEvent in queue or nullptr if empty.
	const GUIEvent getNextGUIEvent(void);

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
	std::queue<GUIEvent> m_guiEvents;
};

// ================================================ //

inline void TFC::enterAsteroidField(void){
	m_inAsteroidField = true;
}

// Getters

inline const int TFC::getNumProbes(void) const{
	return m_probes.size();
}

inline const Uint TFC::getCurrentTime(void) const{
	return m_pTimer->getTicks();
}

inline const GUIEvent TFC::getNextGUIEvent(void){
	if (m_guiEvents.size() == 0){
		GUIEvent next;
		next.type = GUIEventType::NONE;
	}
	else{
		GUIEvent next = m_guiEvents.front();
		m_guiEvents.pop();
	}
}

// Setters


// ================================================ //

#endif

// ================================================ //