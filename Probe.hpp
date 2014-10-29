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
	explicit Probe(const Uint type);

	// Closes the socket.
	~Probe(void);

	// Setup probe data and connect to TFC.
	bool launch(void);

	// Thread which processes probe actions.
	void update(void);

	// Prints an error message with Probe ID.
	void reportError(const char* msg);

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
		NO_TARGET
	};

	// A network message.
	struct Message{
		int type;
		// Use a union to minimize data sent over sockets.
		union{
			struct{
				Uint type;
			} LaunchRequest;

			struct{
				Uint id;

			} ConfirmLaunch;

			Asteroid asteroid;
		};
	};

private:
	Uint m_id;
	Uint m_type;
	Uint m_state;
	SOCKET m_socket;
	struct addrinfo* m_server;	
	std::shared_ptr<Timer> m_pTimer;
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

#endif

// ================================================ //