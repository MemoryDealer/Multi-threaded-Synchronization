// ================================================ //
// File: stdafx.hpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Includes needed libraries.
// ================================================ //

#ifndef __STDAFX_HPP__
#define __STDAFX_HPP__

// ================================================ //

// Define Windows version.
#define STRICT
#define _WIN32_WINNT 0x0600
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

// Some common typedefs.

typedef unsigned int Uint;

// ================================================ //

// C++ STL
#include <memory>
#include <thread>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <cstdio>

// Windows
#include <Windows.h>
#include <CommCtrl.h>
#include <commdlg.h>
#include <Shlwapi.h>
#include <ShlObj.h>

// Network
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>

// ================================================ //

// Converts anything to a std::string.
template<typename T>
static std::string toString(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

// ================================================ //

#endif

// ================================================ //