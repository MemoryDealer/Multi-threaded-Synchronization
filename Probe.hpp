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

// ================================================ //

class Probe
{
public:
	explicit Probe(const unsigned int id,
				   const int ip,
				   const unsigned int type);
	~Probe(void);

	// Setup probe data and connect to TFC.
	int launch(void);

	// Thread which processes probe actions.
	void update(void);

	// Prints an error message with Probe ID.
	void reportError(const char* msg);

	enum Type{
		SCOUT = 1,
		PHOTON,
		PHASER
	};

	enum MessageType{
		LAUNCH = 1
	};

	struct Message{
		int type;
		char buffer[512];
	};

private:
	unsigned int m_id;
	unsigned int m_type;
	SOCKET m_socket;
	bool m_alive;
};

// ================================================ //

#endif

// ================================================ //