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
	explicit IPAddress(void);
	explicit IPAddress(const std::string& host, const int port);
	~IPAddress(void);

	void set(const std::string& host, const int port);

	// Compares the host and port.
	bool operator==(const IPAddress& b){
		return (m_host.compare(b.m_host) == 0 &&
				m_port == b.m_port);
	}

private:
	std::string m_host;
	int m_port;
};

// ================================================ //

inline void IPAddress::set(const std::string& host, const int port){
	m_host = host;
	m_port = port;
}

// ================================================ //