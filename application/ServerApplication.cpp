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

		printf("\nWaiting for client to connect");
		Connection clientConnection = serverSocket.acceptClientConnection();

		bool receivedFullFile = false;
		string fullPayload = "";

		Packet* packet;
		while (!receivedFullFile) {
			packet = serverSocket.receivePacketFromClient(clientConnection.descriptor);	
			fullPayload += string(packet->payload);
			receivedFullFile = (packet->currentPartIndex == packet->numberOfParts);
		}

		printf("\nFull payload is %s", fullPayload.c_str());
		
		Packet answer; 
		strcpy(answer.payload, ("I received file " + string(packet->filename)).c_str());
		serverSocket.sendPacketToClient(clientConnection.descriptor, &answer);
		serverSocket.closeConnection(clientConnection);
	}

	serverSocket.closeSocket();
	return 0; 
}