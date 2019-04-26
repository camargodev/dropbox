#include <stdio.h>
#include "../include/ServerSocketWrapper.hpp"

int main(int argc, char *argv[])
{
	ServerSocketWrapper serverSocket(SocketWrapper::DEFAULT_PORT);
	if (!serverSocket.openSocket()) {
		printf("\nCould not open socket");
		return -1;
	}
	serverSocket.setNumberOfClients(5);
	
	while (true) {
		
		Connection connection = serverSocket.acceptClientConnection();

		Packet* packet = serverSocket.receivePacketFromClient(connection.descriptor);	
		printf("\nThe client sent %s", packet->payload);
		
		Packet answer;
		strcpy(answer.payload, "\nThe server answered 'bye'");
		serverSocket.sendPacketToClient(connection.descriptor, &answer);
		serverSocket.closeConnection(connection);
	}

	serverSocket.closeSocket();
	return 0; 
}