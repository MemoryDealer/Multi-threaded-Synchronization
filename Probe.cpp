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
		m_generator.seed(GetTickCount());
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

	// Connect locally as artificial probe.
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
			std::thread t(&Probe::update, this);
			t.detach();
		}
		else{
			return false;
		}
	}
	else{
		// Connection closed by server.
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
					// Wait random length of time to discover asteroid according to Poisson
					// distribution.															
					Timer::Delay(this->scoutDiscoveryTime());

					// Tell TFC scout is about to report new asteroid.
					msg.type = Probe::MessageType::SCOUT_REQUEST;
					int s = send(m_socket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
					if (s > 0){
						ZeroMemory(&msg, sizeof(msg));
						int r = recv(m_socket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
						if (r > 0){
							// Only proceed with corresponding TFC response.
							if (msg.type != Probe::MessageType::SCOUT_REQUEST){
								break;
							}
						}
						else{
							break;
						}
					}

					// Allocate data for newly discovered asteroid.
					static Uint asteroidIDCtr = 0;
					Asteroid asteroid;
					asteroid.id = asteroidIDCtr++;
					asteroid.discoveryTime = msg.time;

					// Determine asteroid mass based on step function.
					asteroid.mass = this->scoutAsteroidSize();

					// Determine time to impact based on uniform distribution.
					asteroid.impactTime = asteroid.discoveryTime + this->scoutTimeToImpact();

					// Send the asteroid data to TFC.
					ZeroMemory(&msg, sizeof(msg));
					msg.type = Probe::MessageType::ASTEROID_FOUND;
					msg.asteroid = asteroid;
					s = send(m_socket, reinterpret_cast<const char*>(&msg), sizeof(msg), 0);
				}
			}
			break;

		case Probe::Type::PHASER:
		case Probe::Type::PHOTON:
			// Connect to TFC and send defensive request.
			Probe::Message msg;
			msg.id = m_id;
			msg.type = Probe::MessageType::DEFENSIVE_REQUEST;
			int s = send(m_socket, reinterpret_cast<const char*>(&msg), sizeof(msg), 0);
			if (s > 0){
				// Receive response from TFC.
				ZeroMemory(&msg, sizeof(msg));
				int r = recv(m_socket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
				if (r > 0){
					switch (msg.type){
					default:
						break;

					case Probe::MessageType::TARGET_AVAILABLE:
						{
							Uint timeRequired = this->timeRequired(msg.asteroid);

							// See if we have time to destroy the asteroid.
							if (msg.time + timeRequired < msg.asteroid.impactTime){
								printf("Probe %d acquired data for asteroid %d\n\n",
									   m_id, msg.asteroid.id);
								// Destroy the asteroid.
								Timer::Delay(timeRequired);

								// Asteroid destroyed, report to TFC.
								Probe::Message response;
								ZeroMemory(&response, sizeof(response));
								response.type = Probe::MessageType::TARGET_DESTROYED;
								response.id = msg.asteroid.id;
								s = send(m_socket, 
										 reinterpret_cast<const char*>(&response),
										 sizeof(response),
										 0);

								// Allow weapon to recharge.
								Timer::Delay(m_weaponRechargeTime);
							}
							else{
								// Delay any remaining time until impact.
								Timer::Delay((msg.time + timeRequired) - msg.asteroid.impactTime);
								// Then report probe termination and ram the asteroid.
								Probe::Message response;
								ZeroMemory(&response, sizeof(response));
								response.type = Probe::MessageType::TERMINATED;
								response.id = msg.asteroid.id;
								s = send(m_socket, 
										 reinterpret_cast<const char*>(&response),
										 sizeof(response),
										 0);
								m_state = Probe::State::DESTROYED;
								break;
							}
						}
						break;

					case Probe::MessageType::NO_TARGET:
						Timer::Delay(500);
						break;
					}
				}
			}
			break;
		}
	}

	closesocket(m_socket);
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
		time += m_weaponRechargeTime;
		units -= m_weaponPower;				
	}

	return time;
}

// ================================================ //

const Uint Probe::scoutDiscoveryTime(void)
{
	// C++11 method (not used).
	/*std::poisson_distribution<int> poissonDistribution(4.0);
	return (poissonDistribution(m_generator) * 1000);*/

	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	double x = distribution(m_generator);
	double deviate = 0.0;
	if (x == 0.0)     //Interpolate between points on
		deviate = 0.0;  //graph.
	else if (x <= 0.1)
		deviate = ((x - 0.0) * 1.04 + 0.0);
	else if (x <= 0.2)
		deviate = ((x - 0.1) * 1.18 + 0.104);
	else if (x <= 0.3)
		deviate = ((x - 0.2) * 1.33 + 0.222);
	else if (x <= 0.4)
		deviate = ((x - 0.3) * 1.54 + 0.355);
	else if (x <= 0.5)
		deviate = ((x - 0.4) * 1.81 + 0.509);
	else if (x <= 0.6)
		deviate = ((x - 0.5) * 2.25 + 0.690);
	else if (x <= 0.7)
		deviate = ((x - 0.6) * 2.85 + 0.915);
	else if (x <= 0.75)
		deviate = ((x - 0.70) * 3.60 + 1.2);
	else if (x <= 0.8)
		deviate = ((x - 0.75) * 4.40 + 1.38);
	else if (x <= 0.84)
		deviate = ((x - 0.8) * 5.75 + 1.60);
	else if (x <= 0.88)
		deviate = ((x - 0.84) * 7.25 + 1.83);
	else if (x <= 0.9)
		deviate = ((x - 0.88) * 9.00 + 2.12);
	else if (x <= 0.92)
		deviate = ((x - 0.90) * 11.0 + 2.30);
	else if (x <= 0.94)
		deviate = ((x - 0.92) * 14.5 + 2.52);
	else if (x <= 0.95)
		deviate = ((x - 0.94) * 18.0 + 2.81);
	else if (x <= 0.96)
		deviate = ((x - 0.95) * 21.0 + 2.99);
	else if (x <= 0.97)
		deviate = ((x - 0.96) * 30.0 + 3.20);
	else if (x <= 0.98)
		deviate = ((x - 0.97) * 40.0 + 3.50);
	else if (x <= 0.99)
		deviate = ((x - 0.98) * 70.0 + 3.90);
	else if (x <= 0.995)
		deviate = ((x - 0.99) * 140.0 + 4.60);
	else if (x <= 0.998)
		deviate = ((x - 0.995) * 300.0 + 5.30);
	else if (x <= 0.999)
		deviate = ((x - 0.998) * 800.0 + 6.20);
	else
		deviate = ((x - 0.9997) * 1000.0 + 8.0);

	// Calculate with mean of 4.0.
	deviate *= 4.0;

	// Round up?
	//deviate = std::round(deviate);

	// Convert to milliseconds.
	deviate *= 1000.0;
	return static_cast<Uint>(deviate);
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