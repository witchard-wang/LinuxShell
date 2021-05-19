#include "TCPSocket.h"
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <openssl/ssl.h>
#include <vector>
using namespace std;

TCPSocket::TCPSocket(const std::string host, const std::string port){
    struct addrinfo hints, *res;
    // first, load up address structs with getaddrinfo():
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int status;
    //getaddrinfo("www.example.com", "3490", &hints, &res);
    cout << "Connecting to " << host << "..." << endl;
    if ((status = getaddrinfo (host.c_str(), port.c_str(), &hints, &res)) != 0) {
        cerr << "getaddrinfo: " << gai_strerror(status) << endl;
        exit (-1);
    }

    // make a socket:
    cout << "Make a socket" << endl;
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0){
        perror ("Cannot create socket");	
        exit (-1);
    }

    // connect!
    cout << "Connecting..." << endl;
    if (connect(sockfd, res->ai_addr, res->ai_addrlen)<0){
        perror ("Cannot Connect");
        exit (-1);
    }
    cout << "Connected " << endl;
    
	freeaddrinfo(res);
}

void TCPSocket::changeSocket(const std::string host, const std::string port){
    close(sockfd);

    struct addrinfo hints, *res;
    // first, load up address structs with getaddrinfo():
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int status;
    //getaddrinfo("www.example.com", "3490", &hints, &res);
    cout << "Connecting to " << host << "..." << endl;
    if ((status = getaddrinfo (host.c_str(), port.c_str(), &hints, &res)) != 0) {
        cerr << "getaddrinfo: " << gai_strerror(status) << endl;
        exit (-1);
    }

    // make a socket:
    cout << "Make a socket" << endl;
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0){
        perror ("Cannot create socket");	
        exit (-1);
    }

    // connect!
    cout << "Connecting..." << endl;
    if (connect(sockfd, res->ai_addr, res->ai_addrlen)<0){
        perror ("Cannot Connect");
        exit (-1);
    }
    cout << "Connected " << endl;
	freeaddrinfo(res);
}

TCPSocket::~TCPSocket(){
    close(sockfd);
}

int TCPSocket::read(void* msgbuf, int bufcapacity){
    return recv(sockfd, msgbuf, bufcapacity, 0); 
}

int TCPSocket::write(void* msgbuf, int len){
	return send(sockfd, msgbuf, len, 0);
}

int TCPSocket::getfd(){
    return sockfd;
}