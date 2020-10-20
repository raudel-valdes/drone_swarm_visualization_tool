#include <iostream>
#include <string>
#include "socket.h"

// This is a test of the Networking capabilities of communicating between Pis
int NetworkTest()
{
	std::string hostIP;
	std::cout << "Enter host ip address" << std::endl;
	std::cin >> hostIP;

	std::string remoteIP;
	std::cout << "Enter destination ip address" << std::endl;
	std::cin >> remoteIP;

	if (Socket::Init(hostIP.c_str()) < 1)
	{
		std::cout << "Initialization failed" << std::endl;
		return -1;
	}

	if (Socket::SetSockaddrIn(&Socket::remoteAddr, remoteIP.c_str(), Socket::PORTNUM) < 1)
	{
		std::cout << "Set remote failed" << std::endl;
		return -1;
	}

	std::cout << "Init complete" << std::endl;

	char* message;
	bool exit = false;
	while (!exit)
	{
		std::cout << "Menu:" << std::endl
			<< "1) Send message" << std::endl
			<< "2) Recieve message" << std::endl
			<< "3) Exit" << std::endl;

		int menuSelection;
		std::cin >> menuSelection;
		switch (menuSelection)
		{
		case 1:
			std::cout << "Enter message" << std::endl;
			std::cin >> message;
			Socket::SendMessage(message);
			break;

		case 2:
			message = Socket::ReceiveMessage();
			std::cout << "Message: " << message << std::endl;
			break;

		case 3:
			exit = true;
			break;

		default:
			break;
		}
	}
	return 0;
}
