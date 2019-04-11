#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "SocketWrapper.hpp"

#define PORT 4000

int main(int argc, char *argv[])
{
	SocketWrapper serverSocket(PORT);
	serverSocket.setNumberOfClients(5);
	
	while (true) {
		SocketDescriptor connection = serverSocket.acceptConnection();

		string message = serverSocket.readFromConnection(connection);	
		cout << "\nThe client sent me: " << message;
		
		serverSocket.writeToConnection(connection, "\nThe server is sending a bye");

		serverSocket.closeConnection(connection);
	}

	return 0; 
}