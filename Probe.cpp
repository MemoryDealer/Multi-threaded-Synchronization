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

Probe::Probe(const unsigned int id,
			 const int ip,
			 const unsigned int type) :
m_id(id),
m_type(type),
m_socket(INVALID_SOCKET),
m_alive(true)
{

}

// ================================================ //

Probe::~Probe(void)
{
	shutdown(m_socket, SD_SEND);
	closesocket(m_socket);
}

// ================================================ //

int Probe::launch(void)
{
	// Get server address.
	struct addrinfo* result = nullptr;
	struct addrinfo* ptr = nullptr;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int i = getaddrinfo("127.0.0.1", TFC::Port.c_str(), &hints, &result);
	if (i != 0){
		return 1;
	}

	// Create socket.
	ptr = result;
	m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (m_socket == INVALID_SOCKET){
		printf("PROBE: socket() failed: %ld\n", WSAGetLastError());
		return 1;
	}

	// Connect to TFC.
	i = connect(m_socket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
	if (i == SOCKET_ERROR){
		printf("PROBE: connect() failed: %ld\n", WSAGetLastError());
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	// No longer needed.
	freeaddrinfo(result);

	if (m_socket == INVALID_SOCKET){
		printf("PROBE: Unable to connect to server: %ld\n", WSAGetLastError());
		return 1;
	}

	// Send launch request.
	Message msg;
	msg.type = MessageType::LAUNCH;
	i = send(m_socket, reinterpret_cast<const char*>(&msg), sizeof(msg), 0);
	if (i == SOCKET_ERROR){
		printf("PROBE: send() failed: %ld\n", WSAGetLastError());
		closesocket(m_socket);
		return 1;
	}

	printf("Bytes sent: %ld\n", i);
	

	// Wait for confirmation of launch.
	ZeroMemory(&msg, sizeof(msg));
	i = recv(m_socket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
	if (i == SOCKET_ERROR){
		printf("PROBE: recv() failed: %ld\n", WSAGetLastError());
		closesocket(m_socket);
		return 1;
	}
	if (i > 0){
		if (msg.type == MessageType::LAUNCH){
			printf("Probe %d has received confirmation to launch.\n", m_id);
			std::thread t(&Probe::update, this);
			t.detach();
		}
		else{
			printf("Probe %d denied launch.\n", m_id);
		}
	}
	else{
		printf("Connection closed.\n");
	}

	return 0;
}

// ================================================ //

void Probe::update(void)
{
	while (m_alive){
		printf("Probe %d floating...\n", m_id);
		Sleep(500);
	}
}

// ================================================ //

void Probe::reportError(const char* msg)
{

}

// ================================================ //