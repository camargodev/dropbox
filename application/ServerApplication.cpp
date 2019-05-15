#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include "../include/ServerSocketWrapper.hpp"
#include "../include/PacketHandler.hpp"
#include "../include/ConnectionHandler.hpp"

PacketHandler packetHandler;
ConnectionHandler connHandler;
ServerSocketWrapper serverSocket;
FileHandler fileHandler;

int getServerPort(int argc, char *argv[]) {
	int port = SocketWrapper::DEFAULT_PORT;
	if (argc == 2)
        port = stoi(string(argv[1]));
	return port;
}

bool receivedFromTheCurrentOpenSocket(SocketDescriptor originSocket, SocketDescriptor openSocket) {
	return originSocket == openSocket;
}

bool handleReceivedPacket(int socket, Packet* packet) {
    ConnectedClient connectedClient;
	bool shouldKeepExecuting = true;
	switch (packet->command) {
        case UPLOAD_FILE:
			packetHandler.addPacketToReceivedFile(socket, packet->filename, packet);
            if (packet->currentPartIndex == packet->numberOfParts) {
                string content = packetHandler.getFileContent(socket, packet->filename);
                printf("\nI received file %s with payload:\n%s\n", packet->filename, content.c_str());
				connectedClient = connHandler.getConnectedClientBySocket(socket);
				fileHandler.setDirName(connectedClient.username);
				printf("Now I will notify user %s\n", connectedClient.username.c_str());
				for (auto openSocket : connectedClient.openSockets) {
					if (!receivedFromTheCurrentOpenSocket(socket, openSocket))
						serverSocket.sendFileToClient(openSocket, packet->filename);
				}
				fileHandler.createFile(packet->filename, content);
                packetHandler.removeFileFromBeingReceivedList(socket, packet->filename);
            }
			break;

		case DOWNLOAD_REQUISITION:
			printf("\nI'll try to send file %s to client of socket %i\n", packet->payload, socket);
			serverSocket.sendFileToClient(socket, packet->payload);
			break;

		case DELETE_REQUISITION:
			connectedClient = connHandler.getConnectedClientBySocket(socket);
			fileHandler.deleteFile(packet->filename);
			for (auto openSocket : connectedClient.openSockets) {
				Packet answer(DELETE_ORDER, packet->filename);
				serverSocket.sendPacketToClient(openSocket, &answer);
			}
			break;

		case IDENTIFICATION:
            printf("\nClient %s connected on socket %i\n", packet->payload, socket);
						fileHandler.openClientDir(packet->payload);
			connHandler.addSocketToClient(packet->payload, socket);
            break;

		case DISCONNECT:
			connectedClient = connHandler.getConnectedClientBySocket(socket);
			printf("\nClient %s disconnected on socket %i\n", connectedClient.username.c_str(), socket);
			connHandler.removeSocketFromUser(connectedClient.username, socket);
			shouldKeepExecuting = false;
			break;

		case LIST_REQUISITION:
			connectedClient = connHandler.getConnectedClientBySocket(socket);
			char* userDirOnServer = fileHandler.getServerDirectoryNameForUser(connectedClient.username);
			vector<FileForListing> filesOnUserDir = fileHandler.getFilesInDir(userDirOnServer);
			printf("\nI will send %s's file list on socket %i\n", connectedClient.username.c_str(), socket);
			serverSocket.sendFileList(socket, filesOnUserDir);
			break;
    }
	return shouldKeepExecuting;
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
		printf("Could not open socket. Try another port\n");
		return -1;
	}
	serverSocket.setNumberOfClients(5);
	fileHandler.createServerDir();

	while (true) {

		pthread_t connectionThread;
		Connection clientConnection = serverSocket.acceptClientConnection();
		int descriptor = clientConnection.descriptor;
		pthread_create(&connectionThread, NULL, handleNewConnection, &descriptor);

	}

	serverSocket.closeSocket();
	return 0;
}
