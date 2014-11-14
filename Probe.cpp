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
m_state(Probe::State::STANDBY),
m_socket(INVALID_SOCKET),
m_server(nullptr),
m_weaponRechargeTime(0),
m_weaponPower(0),
m_generator()
{
	// Allocate timer for scout probe.
	if (m_type == Probe::Type::SCOUT){		
		m_generator.seed(GetTickCount64());
	}
	else if (m_type == Probe::Type::PHOTON){
		m_weaponRechargeTime = 3000;
		m_weaponPower = 5;
	}
	else if(m_type == Probe::Type::PHASER){
		m_weaponRechargeTime = 2000;
		m_weaponPower = 3;
	}
}

// ================================================ //

Probe::~Probe(void)
{
	closesocket(m_socket);
}

// ================================================ //

bool Probe::launch(void)
{
	// Get server address.
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int i = getaddrinfo("127.0.0.1", TFC::Port.c_str(), &hints, &m_server);
	if (i != 0){
		return false;
	}

	// Create socket.
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
			m_id = msg.id;
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
		switch (m_type){
		default:
			break;

		case Probe::Type::SCOUT:
			{					
				Timer discoveryClock(true);
				Probe::Message msg;
				// Send request with data.
				if (m_state == Probe::State::STANDBY){
					int r = recv(m_socket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
					if (r > 0){
						if (msg.type == Probe::MessageType::SCOUT_REQUEST){
							// TFC has entered asteroid field, begin scouting.
							m_state = Probe::State::ACTIVE;
						}
					}
				}
				else if(m_state == Probe::State::ACTIVE){
					msg.type = Probe::MessageType::SCOUT_REQUEST;
					int s = send(m_socket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
					if (s > 0){
						ZeroMemory(&msg, sizeof(msg));
						int r = recv(m_socket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
						if (r > 0){
							switch (msg.type){
							default:
								break;

							case Probe::MessageType::SCOUT_REQUEST:

								break;
							}
						}
					}

					// Wait random length of time to discover asteroid according to Poisson
					// distribution.															
					Timer::Delay(this->scoutDiscoveryTime());

					// Allocate data for newly discovered asteroid.
					static Uint asteroidIDCtr = 0;
					Asteroid asteroid;
					asteroid.id = asteroidIDCtr++;					
					asteroid.discoveryTime = msg.time + discoveryClock.getTicks();

					// Determine asteroid mass based on step function.
					asteroid.mass = this->scoutAsteroidSize();

					// Determine time to impact based on uniform distribution.
					asteroid.impactTime = asteroid.discoveryTime + this->scoutTimeToImpact();

					// Send the asteroid data to TFC.
					ZeroMemory(&msg, sizeof(msg));
					msg.type = Probe::MessageType::ASTEROID_FOUND;
					msg.asteroid = asteroid;
					s = send(m_socket, reinterpret_cast<const char*>(&msg), sizeof(msg), 0);
					if (s > 0){

					}
				}
			}
			break;

		case Probe::Type::PHASER:
		case Probe::Type::PHOTON:
			// Connect to TFC and send defensive request.
			Probe::Message msg;
			msg.type = Probe::MessageType::DEFENSIVE_REQUEST;
			int s = send(m_socket, reinterpret_cast<const char*>(&msg), sizeof(msg), 0);
			if (s > 0){
				// Receive data.
				ZeroMemory(&msg, sizeof(msg));
				int r = recv(m_socket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
				if (r > 0){
					if (msg.type == Probe::MessageType::TARGET_AVAILABLE){
						// See if we have time to destroy the asteroid.
						Uint timeRequired = this->timeRequired(msg.asteroid);
						printf("------------\nTime required to destroy asteroid: %d\nCurrentTime: %d\nTimeFound: %d\nImpactTime: %d\n------------\n", 
								timeRequired, msg.time, msg.asteroid.discoveryTime, msg.asteroid.impactTime);
						if (msg.time + timeRequired < msg.asteroid.impactTime){
							printf("\tThere is time.\n");

							// Destroy the asteroid.
							Timer::Delay(timeRequired);

							// Asteroid destroyed, report to TFC.
							ZeroMemory(&msg, sizeof(msg));
							msg.type = Probe::MessageType::TARGET_DESTROYED;
							msg.id = m_id;
							s = send(m_socket, reinterpret_cast<const char*>(&msg), sizeof(msg), 0);
							if (s > 0){
											
							}

							// Allow weapon to recharge.
							Timer::Delay(m_weaponRechargeTime);
						}
						else{
							printf("\tNo time! Collision imminent!\n");
							//Timer::Delay(msg.asteroid.impactTime - msg.time);

							// Report termination to TFC.								
							ZeroMemory(&msg, sizeof(msg));
							msg.type = Probe::MessageType::TERMINATED;
							msg.id = m_id;
							s = send(m_socket, reinterpret_cast<const char*>(&msg), sizeof(msg), 0);
							m_state = Probe::State::DESTROYED;								
							break;
						}
					}
					else if(msg.type == Probe::MessageType::NO_TARGET){
						Timer::Delay(500);
					}
				}
			}
			break;
		}
	}

	closesocket(m_socket);
}

// ================================================ //

void Probe::reportError(const char* msg)
{

}

// ================================================ //

const Uint Probe::timeRequired(const Asteroid& a)
{
	int units = a.mass;
	Uint time = 0;
	// Initial hit.
	units -= m_weaponPower;
	// Process additional hits if needed.
	while (units > 0){
		units -= m_weaponPower;
		time += m_weaponRechargeTime;		
	}

	return time;
}

// ================================================ //

const Uint Probe::scoutDiscoveryTime(void)
{
	std::poisson_distribution<int> poissonDistribution(4.0);
	return (poissonDistribution(m_generator) * 1000);
}

// ================================================ //

const Uint Probe::scoutAsteroidSize(void)
{
	std::uniform_real_distribution<double> massDistribution(0.0, 1.0);
	double step = massDistribution(m_generator);
	if (step < 0.2){
		return 3;
	}
	else if (step < 0.5){
		return 6;
	}
	else if (step < 0.7){
		return 9;
	}
	else{
		return 11;
	}
}

// ================================================ //

const Uint Probe::scoutTimeToImpact(void)
{
	std::uniform_real_distribution<double> impactDistribution(0.0, 15.0);
	double time = impactDistribution(m_generator) * 1000.0;
	return static_cast<Uint>(time);
}

// ================================================ //