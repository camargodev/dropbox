#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include "../include/ServerSocketWrapper.hpp"
#include "../include/PacketHandler.hpp"
#include "../include/ConnectionHandler.hpp"

PacketHandler packetHandler;
ConnectionHandler connHandler;
ServerSocketWrapper serverSocket;

int getServerPort(int argc, char *argv[]) {
	int port = SocketWrapper::DEFAULT_PORT;
	if (argc == 2)
        port = stoi(string(argv[1]));
	return port;
}

bool handleReceivedPacket(int socket, Packet* packet) {
    switch (packet->command) {
        case UPLOAD_FILE:
            packetHandler.addPacketToReceivedFile(socket, packet->filename, packet);
            if (packet->currentPartIndex == packet->numberOfParts) {
                string content = packetHandler.getFileContent(socket, packet->filename);
                printf("I received file %s with payload:\n%s\n", packet->filename, content.c_str());
				ConnectedClient client = connHandler.getConnectedClientBySocket(socket);
				printf("Now I should notify user %s with his sockets\n", client.username.c_str());
                packetHandler.removeFileFromBeingReceivedList(socket, packet->filename);
            }
			return true;
        
		case IDENTIFICATION:
            printf("Client %s connected on socket %i\n", packet->payload, socket);
			connHandler.addSocketToClient(packet->payload, socket);
            return true;

		case DISCONNECT:
			ConnectedClient client = connHandler.getConnectedClientBySocket(socket);
			printf("Client %s disconnected on socket %i\n", client.username.c_str(), socket); 
			connHandler.removeSocketFromUser(client.username, socket);
			return false;
    }
}

void *handleNewConnection(void *voidSocket) {
	int socket = *(int*) voidSocket;
	bool shouldKeepExecuting = true;
	while(shouldKeepExecuting) {
		Packet* packet = serverSocket.receivePacketFromClient(socket);
		shouldKeepExecuting = handleReceivedPacket(socket, packet);
	} 
}

int main(int argc, char *argv[]) {

	serverSocket.listenOnPort(getServerPort(argc, argv));

	if (!serverSocket.openSocket()) {
		printf("Could not open socket\n");
		return -1;
	}
	serverSocket.setNumberOfClients(5);

	while (true) {

		printf("Waiting connection\n");
		Connection clientConnection = serverSocket.acceptClientConnection();
		int descriptor = clientConnection.descriptor;
		pthread_t connectionThread;
		printf("Connected with socket %i. Creating new thread...\n", descriptor);
		pthread_create(&connectionThread, NULL, handleNewConnection, &descriptor);

	}

	serverSocket.closeSocket();
	return 0; 
}