#ifndef SOCKET_H_
#define SOCKET_h_

#include <stdint.h>
#include <netinet/in.h>
#include <sys/socket.h>

class Socket
{
public:
	static constexpr int PORTNUM = 1200;
	static constexpr int BUFFERSIZE = 2048;

	static int SetSockaddrIn(struct sockaddr_in* addr, const char* ip, uint16_t port);
	static int Init(const char* IPAddr);
	static int SendMessage(const char* message);
	static char* ReceiveMessage();

	// IPv4 info struct, stores IP addresses
	// The info for this host, use for initialization
	static struct sockaddr_in myAddr;
	static struct sockaddr_in remoteAddr; // Stores info for other unit

	// Might not be needed, only for testing completeness of data, we could make
	// sure sent messages are always constant lengths, pad with 0 if needed?
	static int bytesRecieved;
	static int ourSocketDescriptor; // Descriptor for our socket

	// messageBufferfer to store data, change from character to something else
	// later, message sent is a void *
	static char messageBuffer[BUFFERSIZE];
};

#endif
