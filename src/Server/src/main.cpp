// Server.cpp: definiuje punkt wejścia dla aplikacji.
//

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "HTTPServer.h"

using namespace std;

int main()
{
	HTTPServerOptions options{ "172.28.239.52", 8080 };
	HTTPServer server{ options };
	server.start();

	//if (listen(listenSocket, SOMAXCONN) < 0) {
	//	std::cout << "Error on listening socket!\n";
	//	return 0;
	//}

	//fd_set connections = { 0 };
	//FD_ZERO(&connections);
	//FD_SET(listenSocket, &connections);
	//int fd_max = listenSocket;
	//std::cout << "Version 0.0.1\n";
	//while (true) {
	//	std::cout << "Listening...\n";
	//	fd_set connCopy = connections;
	//	int nConns = select(fd_max + 1, &connCopy, nullptr, nullptr, nullptr);
	//	if (nConns == -1) {
	//		std::cout << "Error on select!\n";
	//		continue;
	//	}
	//	for (int i = 0; i <= fd_max; i++) {
	//		if (!FD_ISSET(i, &connCopy)) {
	//			//std::cout << "Error! Connection is not an valid socket!\n";
	//			continue;
	//		}
	//		if (i == listenSocket) {
	//			int newConn = accept(i, nullptr, nullptr);
	//			if (newConn > 0) {
	//				FD_SET(newConn, &connections);
	//				if (newConn > fd_max) {
	//					fd_max = newConn;
	//				}
	//				std::cout << "New connection arrived! " + std::to_string(newConn) << std::endl;
	//			}
	//			else {
	//				std::cout << "Error on accepting connection!" << std::endl;
	//				close(newConn);
	//			}
	//			continue;
	//		}

	//		char buffer[4096];
	//		int nBytes = recv(i, buffer, 4096, 0);
	//		if (nBytes < 0) {
	//			std::cout << "Error on recv!\n";
	//			close(i);
	//			FD_CLR(i, &connections);
	//			continue;
	//		}
	//		else if (nBytes == 0) {
	//			std::cout << "Connection closed by the client! " + std::to_string(i) << std::endl;
	//			close(i);
	//			FD_CLR(i, &connections);
	//			continue;
	//		}

	//		std::cout << "Got message: " << buffer << "\n";
	//		nBytes = send(i, buffer, 4096, 0);
	//		if (nBytes < 0) {
	//			std::cout << "Error on send!\n";
	//			close(i);
	//			FD_CLR(i, &connections);
	//			continue;
	//		}
	//		else if (nBytes == 0) {
	//			std::cout << "Connection closed by the client! " + std::to_string(i) << std::endl;
	//			close(i);
	//			FD_CLR(i, &connections);
	//			continue;
	//		}
	//		std::cout << "Sent response for " << buffer << "\n";
	//		continue;
	//	}
	//	
	//}
	//return 0;
}
