#include <stdio.h>
#include "include/ServerSocketWrapper.hpp"

int main(int argc, char *argv[])
{
	ServerSocketWrapper serverSocket(SocketWrapper::DEFAULT_PORT);
	serverSocket.setNumberOfClients(5);
	
	while (true) {
		// keeps blocked here until a send ('write' in TCP) is performed at the client
		Connection connection = serverSocket.acceptClientConnection();

		string message = serverSocket.readFromClient(connection.descriptor);	
		printf("\nThe client sent %s", message.c_str());
		
		serverSocket.writeToClient(connection.descriptor, "\nThe server answered 'bye'");
		serverSocket.closeConnection(connection);
	}

	serverSocket.closeSocket();
	return 0; 
}