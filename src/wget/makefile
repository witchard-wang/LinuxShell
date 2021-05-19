all: client
	
TCPSocket.o: TCPSocket.h TCPSocket.cpp
	g++ -g -w -std=c++11 -c TCPSocket.cpp

client: httpclient.cpp TCPSocket.o
	g++ -g -w -std=c++11 -o wget httpclient.cpp TCPSocket.o -fsanitize=address

clean: 
	rm wget