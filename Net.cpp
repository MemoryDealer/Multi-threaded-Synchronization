// ================================================ //
// File: Net.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Implements IPAddress class.
// ================================================ //

#include "Net.hpp"

// ================================================ //

IPAddress::IPAddress(const char* host, const int port) :
m_host(host),
m_port(port)
{

}

// ================================================ //

IPAddress::~IPAddress(void)
{

}

// ================================================ //