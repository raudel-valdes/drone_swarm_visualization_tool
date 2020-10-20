#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "socket.h"

/**
With the IP set command I don't think we need to create sockaddr_in objects for
every drone.
It might cut down on overhead though to create one for each at startup and have
a "SendToAll" type function.
**/

// constexpr int Socket::PORTNUM = 1200;
// constexpr int Socket::BUFFERSIZE = 2048;

struct sockaddr_in Socket::myAddr;
struct sockaddr_in Socket::remoteAddr;

int Socket::bytesRecieved;
int Socket::ourSocketDescriptor;

char Socket::messageBuffer[BUFFERSIZE];

// Sets up the socket with the given IP address and hard coded port. Returns 1
// on success and 0 on error.
int Socket::Init(const char* IPAddr)
{
	// Create the socket: AF_INET = IPv4, SOCK_DGRAM = UDP or SOCK_STREAM=TCP, and 0 is for default
	// flags

	ourSocketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
	if (Socket::ourSocketDescriptor < 0)
	{
		perror("cannot create socket\n");
		return 1;
	}

	SetSockaddrIn(&myAddr, IPAddr, PORTNUM);


	// Bind the socket created earlier to the given myAddr info.
	if (bind(Socket::ourSocketDescriptor, (struct sockaddr*) & myAddr, sizeof(myAddr)) < 0)
	{
		perror("bind failed\n");
		return 1;
	}

	/* not needed for udp
	 if(listen(ourSocketDescriptor,SOMAXCONN)<0)
	 {
	   perror("listen\n");
	   return 1;
	 }*/

	return 0;
}

int Socket::SetSockaddrIn(struct sockaddr_in* addr, const char* ip, uint16_t port)
{
	// bind the socket to given IP address and a specific port
	memset(addr, 0, sizeof(*addr)); // Clear the memory, just in case
	addr->sin_family = AF_INET;  // set the IPv4 address with network endian-ness (presentation to network)
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = inet_addr(ip);
	return 1;/*inet_pton(
		AF_INET, ip,
		&(addr->sin_addr.s_addr))*/ // set port number (host to network short)
}

// Sends a character message to remoteAddr. Returns 0 on failure, 1 on success.
int Socket::SendMessage(const char* message)
{
	// sendto(int socketDescriptor, void * message, length of message, flag
	//	sockaddr * addressToSendTo, sizeof sockaddr)
	// sockaddr is a generic version for ipv4 and ipv6 so we can cast our struct
	// to it
	//socklen_t length = sizeof(remoteAddr);

	if (sendto(ourSocketDescriptor, message, strlen((char*)message), MSG_DONTWAIT, (struct sockaddr*) & remoteAddr, sizeof(remoteAddr)) < 0)
	{
		perror("sendto failed");
		return 0;
	}

	return 1;
}

// Recieves a message sent to the address in myAddr. Returns NULL on failure.
char* Socket::ReceiveMessage()
{
	// recieve message from socket
	// Parameters: socket, buffer, buffer length, flags, struct to store info on
	// sender, size of that struct
	socklen_t length = sizeof(myAddr);

	bytesRecieved = recvfrom(ourSocketDescriptor, messageBuffer, BUFFERSIZE, MSG_DONTWAIT, (struct sockaddr*) & remoteAddr, &length);
	//printf("received %d bytes\n", bytesRecieved);
	if (bytesRecieved > 0)
	{
		messageBuffer[bytesRecieved] = 0;
		return messageBuffer;
	}

	return NULL;
}
