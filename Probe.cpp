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
m_socket(INVALID_SOCKET)
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
		printf("ERROR on probe socket(): %ld\n", WSAGetLastError());
		return 1;
	}

	// Connect to TFC.
	i = connect(m_socket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
	if (i == SOCKET_ERROR){
		printf("connect() failed: %ld\n", WSAGetLastError());
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	// No longer needed.
	freeaddrinfo(result);

	if (m_socket == INVALID_SOCKET){
		printf("Unable to connect to server: %ld\n", WSAGetLastError());
		return 1;
	}

	// Send launch request.
	Message msg;
	msg.type = MessageType::LAUNCH + 1;
	i = send(m_socket, reinterpret_cast<const char*>(&msg), sizeof(msg), 0);
	if (i == SOCKET_ERROR){
		printf("send() failed: %ld\n", WSAGetLastError());
		closesocket(m_socket);
		return 1;
	}

	printf("Bytes sent: %ld\n", i);

	return 0;
}

// ================================================ //

void Probe::update(void)
{

}

// ================================================ //