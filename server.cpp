#include <stdio.h>
#include "SocketWrapper.hpp"

int main(int argc, char *argv[])
{
	SocketWrapper serverSocket;
	serverSocket.configureAsServer(SocketWrapper::DEFAULT_PORT);
	serverSocket.setNumberOfClients(5);
	
	while (true) {
		// keeps blocked here until a send (write in TCP) is performed at the client
		Connection connection = serverSocket.acceptConnection();

		string message = serverSocket.readFromClient(connection);	
		printf("\nThe client sent %s", message.c_str());
		
		serverSocket.writeToClient(connection, "\nThe server answered 'bye'");
		serverSocket.closeConnection(connection);
	}

	serverSocket.closeSocket();
	return 0; 
}