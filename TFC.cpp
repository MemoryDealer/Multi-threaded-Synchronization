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

TFC::TFC(void) :
m_asteroids(),
m_probes(),
m_socket(INVALID_SOCKET),
m_fleetAlive(true),
m_inAsteroidField(false)
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

	// Spawn a thread to accept new probe connections.
	std::thread t(&TFC::launchProbes, this);
	// Allow continued execution of calling thread.
	t.detach(); 

	return 0;
}

// ================================================ //

void TFC::launchProbes(void)
{
	// Don't allow new probes to launch once we've entered the asteroid field.
	while (!m_inAsteroidField){
		// Accept initial probe connections.
		struct sockaddr_in probeInfo = { 0 };
		int size = sizeof(probeInfo);
		SOCKET probeSocket = accept(m_socket, reinterpret_cast<struct sockaddr*>(&probeInfo), &size);
		if (probeSocket == INVALID_SOCKET){
			printf("TFC: accept() failed: %ld\n", WSAGetLastError());
			closesocket(probeSocket);
			continue;
		}

		// Display new connection info.
		char* host = inet_ntoa(probeInfo.sin_addr);
		int port = ntohs(probeInfo.sin_port);
		IPAddress ip(host, port);
		printf("New Probe: %s:%d\n", host, port);

		// Receive probe launch request.
		int r = 0;
		Probe::Message msg;
		r = recv(probeSocket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
		if (r > 0){
			if (msg.type == Probe::MessageType::LAUNCH){
				printf("Launch request received.\n");

				Probe::Message confirm;
				confirm.type = Probe::MessageType::LAUNCH - 1;

				int s = send(probeSocket, reinterpret_cast<const char*>(&confirm), sizeof(confirm), 0);
				if (s > 0){
					printf("Sent launch confirmation.\n");
				}
			}
			closesocket(probeSocket);
			r = 0;
		}
	}
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
	int r = 5;
	while (r > 0){
		
	}

	return 0;
}

// ================================================ //