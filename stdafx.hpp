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
#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN

// Some common typedefs.


// ================================================ //

// C++ STL
#include <memory>
#include <thread>
#include <string>
#include <vector>
#include <list>
#include <cstdio>

// Windows
#include <Windows.h>

// Network
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>

// ================================================ //

#endif

// ================================================ //