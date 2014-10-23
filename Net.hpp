// ================================================ //
// File: Net.hpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Defines IPAddress class.
// ================================================ //

#include "stdafx.hpp"

// ================================================ //

class IPAddress
{
public:
	explicit IPAddress(const char* host, const int port);
	~IPAddress(void);

	// Compares the host and port.
	bool operator==(const IPAddress& b){
		return (strcmp(m_host, b.m_host) == 0 &&
				m_port == b.m_port);
	}

private:
	const char* m_host;
	int m_port;
};

// ================================================ //