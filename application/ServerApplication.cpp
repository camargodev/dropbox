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
                printf("\nI received file %s with payload:\n%s\n", packet->filename, content.c_str());
				ConnectedClient connectedClient = connHandler.getConnectedClientBySocket(socket);
				printf("Now I will notify user %s\n", connectedClient.username.c_str());
				for (auto socket : connectedClient.openSockets) {
					Packet answer(SIMPLE_MESSAGE);
					strcpy(answer.payload, ("You have an update on file " + string(packet->filename)).c_str());
					serverSocket.sendPacketToClient(socket, &answer);
				}
                packetHandler.removeFileFromBeingReceivedList(socket, packet->filename);
            }
			return true;
        
		case IDENTIFICATION:
            printf("Client %s connected on socket %i\n", packet->payload, socket);
			connHandler.addSocketToClient(packet->payload, socket);
            return true;

		case DISCONNECT:
			ConnectedClient client = connHandler.getConnectedClientBySocket(socket);
			printf("\nClient %s disconnected on socket %i\n", client.username.c_str(), socket); 
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

		pthread_t connectionThread;
		Connection clientConnection = serverSocket.acceptClientConnection();
		int descriptor = clientConnection.descriptor;
		printf("\nConnection detected. Creating new thread...\n");
		pthread_create(&connectionThread, NULL, handleNewConnection, &descriptor);

	}

	serverSocket.closeSocket();
	return 0; 
}