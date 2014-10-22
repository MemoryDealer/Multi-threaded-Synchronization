// ================================================ //
// File: TFC.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Implements TFC (Task Force Command) class.
// ================================================ //

#include "TFC.hpp"

// ================================================ //

const std::string TFC::Port = "27666";

// ================================================ //

TFC::TFC(const unsigned int numPhaserProbes) :
m_asteroids(),
m_probes(),
m_socket(INVALID_SOCKET),
m_fleetAlive(true),
m_exitedAsteroidField(false)
{
	int ret = this->init();
	printf("setupServer() = %d\n", ret);
}

// ================================================ //

TFC::~TFC(void)
{
	closesocket(m_socket);
	WSACleanup();
}

// ================================================ //

int TFC::init(void)
{
	struct addrinfo* result = nullptr;
	struct addrinfo* ptr = nullptr;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve local address and port.
	int i = getaddrinfo(nullptr, TFC::Port.c_str(), &hints, &result);
	if (i != 0){
		return i;
	}

	// Create a socket for server to listen for connections.
	m_socket = socket(result->ai_family, 
					  result->ai_socktype, 
					  result->ai_protocol);
	if (m_socket == INVALID_SOCKET){
		freeaddrinfo(result);
		return i;
	}

	// Bind the socket to network address.
	i = bind(m_socket, result->ai_addr, static_cast<int>(result->ai_addrlen));
	if (i == SOCKET_ERROR){
		freeaddrinfo(result);
		closesocket(m_socket);
		return i;
	}

	// This is no longer needed, reclaim.
	freeaddrinfo(result);

	// Begin listening on socket for incoming connections.
	if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR){
		closesocket(m_socket);
		return SOCKET_ERROR;
	}

	std::thread t(&TFC::launchProbes, this);
	t.detach();

	return 0;
}

// ================================================ //

void TFC::initProbeLaunching(void)
{
	std::thread t(&TFC::launchProbes, this);
	t.detach();
}

// ================================================ //

int TFC::launchProbes(void)
{
	while (true){
		// Accept initial probe connections.
		SOCKET newProbeSocket = accept(m_socket, nullptr, nullptr);
		if (newProbeSocket == INVALID_SOCKET){
			closesocket(m_socket);
			return 1;
		}

		printf("Connection accepted...\n");

		// Receive probe data.
		int r = 5;
		Probe::Message msg;
		while (r > 1){
			r = recv(newProbeSocket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
			if (r > 0){
				printf("Bytes received: %d\ntype=%d\n", r, msg.type);
				closesocket(newProbeSocket);
				r = 0;
			}
		}
	}

	return 0;
}

// ================================================ //

int TFC::navigateAsteroidField(void)
{
	// Accept incoming connections.
	SOCKET clientSocket = accept(m_socket, nullptr, nullptr);
	if (clientSocket == INVALID_SOCKET){
		closesocket(m_socket);
		return 1;
	}

	// Receive the data from newly connected client.
	int r = 1;
	while (r > 0){
		
	}

	return 0;
}

// ================================================ //