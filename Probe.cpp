// ================================================ //
// File: Probe.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Implements Probe class.
// ================================================ //

#include "Probe.hpp"
#include "TFC.hpp"

// ================================================ //

Probe::Probe(const Uint type) :
m_id(0),
m_type(type),
m_state(Probe::State::ACTIVE),
m_socket(INVALID_SOCKET),
m_server(nullptr)
{

}

// ================================================ //

Probe::~Probe(void)
{
	shutdown(m_socket, SD_SEND);
	closesocket(m_socket);
}

// ================================================ //

bool Probe::launch(void)
{
	// Get server address.
	struct addrinfo* result = nullptr;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int i = getaddrinfo("127.0.0.1", TFC::Port.c_str(), &hints, &result);
	if (i != 0){
		return false;
	}

	// Create socket.
	m_server = result;
	m_socket = socket(m_server->ai_family, m_server->ai_socktype, m_server->ai_protocol);
	if (m_socket == INVALID_SOCKET){
		printf("PROBE: socket() failed: %ld\n", WSAGetLastError());
		return false;
	}

	// Connect to TFC.
	i = connect(m_socket, m_server->ai_addr, static_cast<int>(m_server->ai_addrlen));
	if (i == SOCKET_ERROR){
		printf("PROBE: connect() failed: %ld\n", WSAGetLastError());
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	// No longer needed.
	//freeaddrinfo(result);

	if (m_socket == INVALID_SOCKET){
		printf("PROBE: Unable to connect to server: %ld\n", WSAGetLastError());
		return false;
	}

	// Send launch request.
	Message msg;
	msg.type = MessageType::LAUNCH_REQUEST;
	msg.LaunchRequest.type = m_type;
	
	i = send(m_socket, reinterpret_cast<const char*>(&msg), sizeof(msg), 0);
	if (i == SOCKET_ERROR){
		printf("PROBE: send() failed: %ld\n", WSAGetLastError());
		closesocket(m_socket);
		return false;
	}

	printf("Bytes sent: %ld\n", i);
	

	// Wait for confirmation of launch.
	ZeroMemory(&msg, sizeof(msg));
	i = recv(m_socket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
	if (i == SOCKET_ERROR){
		printf("PROBE: recv() failed: %ld\n", WSAGetLastError());
		closesocket(m_socket);
		return false;
	}
	if (i > 0){
		if (msg.type == MessageType::CONFIRM_LAUNCH){
			// Launch confirmed, save ID assigned by TFC.
			m_id = msg.ConfirmLaunch.id;
			printf("Probe %d has received confirmation to launch.\n", m_id);
			std::thread t(&Probe::update, this);
			t.detach();
		}
		else{
			printf("Probe %d denied launch.\n", m_id);
			return false;
		}
	}
	else{
		printf("Connection closed.\n");
		return false;
	}

	return true;
}

// ================================================ //

void Probe::update(void)
{
	while (m_state != Probe::State::DESTROYED){		
		if (m_state == Probe::State::ACTIVE){
			
			switch (m_type){
			default:
				break;

			case Probe::Type::SCOUT:
				// Connect to TFC and send scout request.
				m_socket = socket(m_server->ai_family, m_server->ai_socktype, m_server->ai_protocol);
				if (connect(m_socket, m_server->ai_addr,
					static_cast<int>(m_server->ai_addrlen)) != SOCKET_ERROR){
					// Send request with data.
					Probe::Message msg;
					msg.type = Probe::MessageType::SCOUT_REQUEST;
					int s = send(m_socket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
					if (s > 0){
						ZeroMemory(&msg, sizeof(msg));
						int r = recv(m_socket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
						if (r > 0){
							switch (msg.type){
							default:
								break;

							case 0:
								break;
							}
						}
					}
				}
				closesocket(m_socket);

				// Wait random length of time.
				// ...
				Sleep(500);

				{
					Asteroid asteroid;
					asteroid.discoveryTime = 0;
					asteroid.mass = 5;

					m_socket = socket(m_server->ai_family, m_server->ai_socktype, m_server->ai_protocol);
					if (connect(m_socket, m_server->ai_addr,
						static_cast<int>(m_server->ai_addrlen)) != SOCKET_ERROR){
						Probe::Message msg;
						msg.type = Probe::MessageType::ASTEROID_FOUND;
						msg.asteroid = asteroid;
						int s = send(m_socket, reinterpret_cast<const char*>(&msg), sizeof(msg), 0);
						if (s > 0){
							printf("Asteroid info sent.\n");
						}
					}
				}
				closesocket(m_socket);
				break;

			case Probe::Type::PHASER:
			case Probe::Type::PHOTON:
				// Connect to TFC and send defensive request.
				m_socket = socket(m_server->ai_family, m_server->ai_socktype, m_server->ai_protocol);
				if (connect(m_socket, m_server->ai_addr,
					static_cast<int>(m_server->ai_addrlen)) != SOCKET_ERROR){
					Probe::Message msg;
					msg.type = Probe::MessageType::DEFENSIVE_REQUEST;
					int s = send(m_socket, reinterpret_cast<const char*>(&msg), sizeof(msg), 0);
					if (s > 0){
						// Receive data.
						ZeroMemory(&msg, sizeof(msg));
						int r = recv(m_socket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
						if (r > 0){
							printf("Defensive probe received: %d\n", msg.type);
						}
					}
				}
				Sleep(500);
				closesocket(m_socket);
				break;
			}
		}
	}
}

// ================================================ //

void Probe::reportError(const char* msg)
{

}

// ================================================ //