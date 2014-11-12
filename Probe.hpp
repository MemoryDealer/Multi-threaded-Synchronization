// ================================================ //
// File: Probe.hpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Defines Probe class.
// ================================================ //

#ifndef __PROBE_HPP__
#define __PROBE_HPP__

// ================================================ //

#include "stdafx.hpp"
#include "Asteroid.hpp"

class Timer;

// ================================================ //

class Probe
{
public:
	// Default initializes all member variables.
	explicit Probe(void);

	// Closes the socket.
	virtual ~Probe(void);

	// Setup probe data and connect to TFC.
	bool launch(void);

	// Thread which processes probe actions.
	virtual void update(void) = 0;

	// Prints an error message with Probe ID.
	void reportError(const char* msg);

	// Returns time required in milliseconds to destroy Asteroid a.
	const Uint timeRequired(const Asteroid& a);

	// Getters

	// Returns Probe ID in string format.
	const Uint getID(void) const;

	// Returns current Probe state in string format.
	const Uint getState(void) const;

	enum Type{
		SCOUT = 1,
		PHOTON,
		PHASER
	};

	enum State{
		DESTROYED = 0,
		STANDBY,
		ACTIVE
	};

	enum MessageType{
		LAUNCH_REQUEST = 1,
		CONFIRM_LAUNCH,
		SCOUT_REQUEST,
		DEFENSIVE_REQUEST,
		ASTEROID_FOUND,
		TARGET_AVAILABLE,
		NO_TARGET,
		TARGET_DESTROYED,
		TERMINATED
	};

	// A network message.
	struct Message{
		int type;
		Uint time;
		// Use a union to minimize data sent over sockets.
		union{
			struct{
				Uint type;
			} LaunchRequest;

			Uint id;

			Asteroid asteroid;
		};
	};

private:
	Uint m_id;
	Uint m_type;
	Uint m_state;
	SOCKET m_socket;
	struct addrinfo* m_server;	
	int m_weaponRechargeTime;
	int m_weaponPower;
	std::default_random_engine m_generator;
};

// ================================================ //

// Getters

inline const Uint Probe::getID(void) const{
	return m_id;
}

inline const Uint Probe::getState(void) const{
	return m_state;
}

// ================================================ //
// ================================================ //

class ScoutProbe : public Probe
{
public:
	explicit ScoutProbe(void);

private:
	std::shared_ptr<Timer> m_pClock;
};

// ================================================ //

#endif

// ================================================ //