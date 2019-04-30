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
    ConnectedClient connectedClient;
	switch (packet->command) {
        case UPLOAD_FILE:
            packetHandler.addPacketToReceivedFile(socket, packet->filename, packet);
            if (packet->currentPartIndex == packet->numberOfParts) {
                string content = packetHandler.getFileContent(socket, packet->filename);
                printf("\nI received file %s with payload:\n%s\n", packet->filename, content.c_str());
				connectedClient = connHandler.getConnectedClientBySocket(socket);
				printf("Now I will notify user %s\n", connectedClient.username.c_str());
				for (auto openSocket : connectedClient.openSockets) {
					serverSocket.sendFileToClient(openSocket, packet->filename);
				}
                packetHandler.removeFileFromBeingReceivedList(socket, packet->filename);
            }
			return true;

		case DOWNLOAD_REQUISITION:
			printf("\nI'll try to send file %s to client of socket %i\n", packet->payload, socket);
			serverSocket.sendFileToClient(socket, packet->payload);
			return true;

		case DELETE_REQUISITION:
			connectedClient = connHandler.getConnectedClientBySocket(socket);	
			printf("\nI should now delete the file %s on /sync_dir_%s\n", packet->filename, connectedClient.username.c_str());
			for (auto openSocket : connectedClient.openSockets) {
				Packet answer(DELETE_ORDER, packet->filename);
				serverSocket.sendPacketToClient(openSocket, &answer);
			}
			break;
        
		case IDENTIFICATION:
            printf("\nClient %s connected on socket %i\n", packet->payload, socket);
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
		pthread_create(&connectionThread, NULL, handleNewConnection, &descriptor);

	}

	serverSocket.closeSocket();
	return 0; 
}