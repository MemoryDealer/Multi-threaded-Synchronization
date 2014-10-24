// ================================================ //
// File: TFC.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Implements TFC (Task Force Command) class.
// ================================================ //

#include "TFC.hpp"
#include "resource.h"

// ================================================ //

const std::string TFC::Port = "27876";

// ================================================ //

TFC::TFC(void) :
m_asteroids(),
m_probes(),
m_socket(INVALID_SOCKET),
m_fleetAlive(true),
m_inAsteroidField(false),
m_shields(5),
m_asteroidsDestroyed(0)
{
	int ret = this->init();
	printf("setupServer() = %d\n", ret);
}

// ================================================ //

TFC::~TFC(void)
{
	closesocket(m_socket);
}

// ================================================ //

int TFC::init(void)
{
	struct addrinfo* result = nullptr;
	struct addrinfo* ptr = nullptr;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = PF_INET;
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
	Uint probeIDCtr = 1;

	// Don't allow new probes to launch once we've entered the asteroid field.
	while (m_inAsteroidField == false){
		// Accept initial probe connections.
		struct sockaddr_in probeInfo = { 0 };
		int size = sizeof(probeInfo);
		SOCKET probeSocket = accept(m_socket, reinterpret_cast<struct sockaddr*>(&probeInfo), &size);
		if (probeSocket == INVALID_SOCKET){
			printf("TFC: accept() failed: %ld\n", WSAGetLastError());
			closesocket(probeSocket);
			continue;
		}

		if (m_inAsteroidField){
			printf("Returning from launch thread*****************\n");
			return;
		}

		// Display new connection info.
		char* host = inet_ntoa(probeInfo.sin_addr);
		int port = ntohs(probeInfo.sin_port);
		IPAddress ip;
		ip.set(std::string(host), port);
		printf("New Probe: %s:%d\n", host, port);

		// Receive probe launch request.
		int r = 0;
		Probe::Message msg;
		r = recv(probeSocket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
		if (r > 0){
			if (msg.type == Probe::MessageType::LAUNCH_REQUEST){
				printf("Launch request received.\n");

				// Send a launch confirmation back to the probe, as well as the ID.
				Probe::Message confirm;
				confirm.type = Probe::MessageType::CONFIRM_LAUNCH;
				confirm.ConfirmLaunch.id = probeIDCtr++;

				int s = send(probeSocket, reinterpret_cast<const char*>(&confirm), sizeof(confirm), 0);
				if (s > 0){
					printf("Sent launch confirmation, adding to probe list.\n");

					// Add probe to TFC list of probes.
					ProbeRecord probe;
					probe.id = confirm.ConfirmLaunch.id;
					probe.ip = ip;
					probe.type = msg.LaunchRequest.type;
					probe.state = 0;					
					m_probes.push_back(probe);
				}
			}			
		}

		// Terminate connection.
		closesocket(probeSocket);
	}
}

// ================================================ //

void TFC::enterAsteroidField(void)
{
	std::thread t(&TFC::navigateAsteroidField, this);
	t.detach();
}

// ================================================ //

int TFC::navigateAsteroidField(void)
{
	m_inAsteroidField = true;

	while (m_inAsteroidField == true){
		// Accept incoming probe requests.
		struct sockaddr_in probeInfo = { 0 };
		int size = sizeof(probeInfo);
		SOCKET probeSocket = accept(m_socket, reinterpret_cast<struct sockaddr*>(&probeInfo), &size);
		if (probeSocket == INVALID_SOCKET){
			printf("TFC: accept() failed: %ld\n", WSAGetLastError());
			closesocket(probeSocket);
			continue;
		}

		char* host = inet_ntoa(probeInfo.sin_addr);
		int port = ntohs(probeInfo.sin_port);
		IPAddress ip(host, port);

		// See if this is a launched probe.
		bool launched = false;
		for (std::vector<ProbeRecord>::iterator itr = m_probes.begin();
			 itr != m_probes.end();
			 ++itr){
			// Re-opened sockets have a new port :(
			//if (ip == itr->ip){
				launched = true;
			//}
		}

		// If not, close connection and accept the next request.
		if (launched == false){
			closesocket(probeSocket);
			continue;
		}

		// Receive the request.
		int r = 0;
		Probe::Message msg;
		r = recv(probeSocket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
		if (r > 0){
			switch (msg.type){
			default:
				break;

			case Probe::MessageType::SCOUT_REQUEST:
				printf("SCOUT REQUEST RECEIVED...\n");
				break;

			case Probe::MessageType::DEFENSIVE_REQUEST:

				break;
			}
		}
		else{
			// Report error...
		}

		// Update.
		if (m_asteroidsDestroyed > 55){
			m_inAsteroidField = false;
		}
		else{
			if (m_shields <= 0){
				m_inAsteroidField = false;
				m_fleetAlive = false;
			}
		}
	}

	if (m_fleetAlive){
		printf("FLEET MADE IT!\n");
	}
	else{
		printf("FLEET DEAD\n");
	}

	return 0;
}

// ================================================ //