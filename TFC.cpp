// ================================================ //
// File: TFC.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Implements TFC (Task Force Command) class.
// ================================================ //

#include "TFC.hpp"
#include "Timer.hpp"
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
m_asteroidsDestroyed(0),
m_pTimer(new Timer()),
m_guiEvents()
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
	std::thread t(&TFC::update, this);
	// Allow continued execution of calling thread.
	t.detach(); 

	return 0;
}

// ================================================ //

void TFC::update(void)
{
	m_pTimer->restart();

	while (m_fleetAlive == true){
		// Accept incoming probe requests.
		struct sockaddr_in probeInfo = { 0 };
		int size = sizeof(probeInfo);
		SOCKET probeSocket = accept(m_socket, reinterpret_cast<struct sockaddr*>(&probeInfo), &size);
		if (probeSocket == INVALID_SOCKET){
			printf("TFC: accept() failed: %ld\n", WSAGetLastError());
			closesocket(probeSocket);
			continue;
		}

		// Store remote IP address and port.
		char* host = inet_ntoa(probeInfo.sin_addr);
		int port = ntohs(probeInfo.sin_port);
		IPAddress ip(host, port);

		// Receive the request.
		int r = 0;
		Probe::Message msg;
		r = recv(probeSocket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
		if (r > 0){
			if (m_inAsteroidField == false){
				if (msg.type == Probe::MessageType::LAUNCH_REQUEST){
					printf("Launch request received from %s:%d.\n", host, port);

					// Send a launch confirmation back to the probe, as well as the ID.
					static Uint probeIDCtr = 0;
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
			// Process messages while navigating asteroid field.
			else{
				switch (msg.type){
				default:
					break;

				case Probe::MessageType::LAUNCH_REQUEST:
					// Only accept launch requests when not in asteroid field.					
					break;

				case Probe::MessageType::SCOUT_REQUEST:
				{
					Probe::Message response;
					// Ack request.
					response.type = 0;
					int s = send(probeSocket, reinterpret_cast<const char*>(&response), sizeof(response), 0);
					if (s > 0){
						
					}
				}
					break;

				case Probe::MessageType::DEFENSIVE_REQUEST:
				{
					Probe::Message response;
					if (m_asteroids.empty()){
						response.type = Probe::MessageType::NO_TARGET;
					}
					else{
						response.type = Probe::MessageType::TARGET_AVAILABLE;
						response.asteroid = m_asteroids.remove();

						// Trigger GUI event to remove asteroid from listview.
						GUIEvent e;
						e.type = GUIEventType::ASTEROID_REMOVED;
						e.x = response.asteroid.id;
						m_guiEvents.push(e);
					}

					// Test
					int s = send(probeSocket, reinterpret_cast<const char*>(&response), sizeof(response), 0);
					if (s > 0){
						
					}
				}
					break;

				case Probe::MessageType::ASTEROID_FOUND:

					if (m_asteroids.insert(msg.asteroid))					
					{
						// Inform main GUI of new asteroid.
						GUIEvent e;
						e.type = GUIEventType::ASTEROID_FOUND;
						e.asteroid = msg.asteroid;
						m_guiEvents.push(e);
					}
					else{
						--m_shields;
						GUIEvent e;
						e.type = GUIEventType::SHIELDS_HIT;
						m_guiEvents.push(e);
					}
					break;
				}
			}
		}
		else{
			// Report error...
		}

		closesocket(probeSocket);

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
}

// ================================================ //