#ifndef _TCPSocket_H_
#define _TCPSocket_H_

#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <string>
class TCPSocket
{	
private:
	int sockfd;
	
public:
	TCPSocket(const std::string host, const std::string port);
	TCPSocket(int);
	~TCPSocket();

	void changeSocket(const std::string host, const std::string port);

	int write(void* msgbuf, int bufcapacity);

	int read(void *msgbuf , int msglen);

	int getfd();
};

#endif
