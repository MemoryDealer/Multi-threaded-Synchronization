// ================================================ //
// File: main.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Defines function main(), entry point of program.
// ================================================ //

#include "TFC.hpp"
#include "Probe.hpp"

// ================================================ //

int main(int argc, char** argv)
{
	// Initialize Winsock, begin using WS2_32.DLL.
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
		return 1;
	}

	// Initialize TFC (server).
	TFC tfc;

	int probeID = 0;
	// Prepare probes and launch them.
	while (tfc.getNumProbes() < 12){
		Probe* probe = new Probe(probeID++, 0, Probe::Type::PHASER);
		probe->launch();
		printf("Waiting for probes...\n");
		Sleep(5000);

	}

	// Enter the asteroid field.
	while (true){
		tfc.navigateAsteroidField();
	}

	// Terminate use of WS2_32.DLL.
	WSACleanup();

	return 0;
}

// ================================================ //