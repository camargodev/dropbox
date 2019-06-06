#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include "../include/ServerSocketWrapper.hpp"
#include "../include/ClientSocketWrapper.hpp"
#include "../include/ConnectionHandler.hpp"
#include "../include/ServerFileHandler.hpp"
#include "../include/ReplicationHelper.hpp"

ConnectionHandler connHandler;
ServerSocketWrapper serverSocket;
ClientSocketWrapper clientSocket;
ServerFileHandler fileHandler;
ReplicationHelper replicationHelper;

int getServerPort(int argc, char *argv[]) {
	int port = stoi(string(argv[1]));
    printf("I will answer on port %i\n", port);
	return port;
}

bool receivedFromTheCurrentOpenSocket(SocketDescriptor originSocket, SocketDescriptor openSocket) {
	return originSocket == openSocket;
}

bool handleReceivedPacket(int socket, Packet* packet) {
    ConnectedClient connectedClient = connHandler.getConnectedClientBySocket(socket);

    bool shouldKeepExecuting = true;
	string filenameToSave;

    printf("I received a packet!!\n");
	switch (packet->command) {
        case MIRROR: {
            printf("I will send everything to my mirror on socket %i\n", socket);
            replicationHelper.addMirror(socket);
            Packet packet(SIMPLE_MESSAGE, 40, (char*) "Response from server");
            if (serverSocket.sendPacketToClient(socket, &packet)) {
                printf("I sent a ack to my mirror\n");
            }
            break;
        }

        case SIMPLE_MESSAGE:
            printf("I received a simple message: %s\n", packet->payload);
            break;

		case GET_SYNC_DIR: {
            printf("Client %s requested sync dir\n", connectedClient.username.c_str());
            vector <FileForListing> filesOnUserDirectory = fileHandler.getFiles(connectedClient.username.c_str());

            printf("\nI will send %s's file list on socket %i\n", connectedClient.username.c_str(), socket);
            serverSocket.sendFileListForSyncDir(socket, filesOnUserDirectory);

            break;
        }

        case UPLOAD_FILE: {
            if(packet->currentPartIndex == 1)
                fileHandler.createFile(connectedClient.username.c_str(), packet->filename, packet->payload, packet->payloadSize);
            else
                fileHandler.appendFile(connectedClient.username.c_str(), packet->filename, packet->payload, packet->payloadSize);

            if (replicationHelper.isMainServer()) {
                if (packet->currentPartIndex == packet->numberOfParts) {
                    WrappedFile file = fileHandler.getFile(connectedClient.username.c_str(), packet->filename);
                    for (auto openSocket : connectedClient.openSockets) {
                        if (!receivedFromTheCurrentOpenSocket(socket, openSocket))
                            serverSocket.sendSyncFile(openSocket, file);
                    }
                }
                for (auto mirror : replicationHelper.getMirrors())
                    serverSocket.sendPacketToClient(mirror, packet);
            }

            break;
        }

		case DOWNLOAD_REQUISITION: {
            printf("\nI'll try to send file %s to client of socket %i\n", packet->payload, socket);

            WrappedFile file = fileHandler.getFile(connectedClient.username.c_str(), packet->payload);
            serverSocket.sendDownloadedFile(socket, file);

            break;
        }

        case ASK_FOR_SYNC_FILE: {
            printf("\nI'll try to send file %s to client of socket %i\n", packet->payload, socket);

            WrappedFile file = fileHandler.getFile(connectedClient.username.c_str(), packet->payload);
            serverSocket.sendSyncFile(socket, file);

            break;

        }

		case DELETE_REQUISITION: {
            fileHandler.deleteFile(connectedClient.username.c_str(), packet->filename);

            if (replicationHelper.isMainServer()) {
                Packet answer(DELETE_ORDER, packet->filename);
                for (auto openSocket : connectedClient.openSockets) {
                    if (!receivedFromTheCurrentOpenSocket(socket, openSocket))
                        serverSocket.sendPacketToClient(openSocket, &answer);
                }
                for (auto mirror : replicationHelper.getMirrors()) 
                    serverSocket.sendPacketToClient(mirror, packet);
            }
            break;
        }

		case IDENTIFICATION: {
            printf("\nClient %s connected on socket %i with IP %s\n", packet->payload, socket, packet->ip);

            fileHandler.createClientDir(packet->payload);
            connHandler.addSocketToClient(packet->payload, socket);

            if (replicationHelper.isMainServer()) {
                printf("I will send this identification to all my mirrors\n");
                for (auto mirror : replicationHelper.getMirrors()) {
                    if (serverSocket.sendPacketToClient(mirror, packet))
                        printf("Sent with success to mirror %i\n", mirror);                    
                }
            }
            break;
        }

		case DISCONNECT: {
            printf("\nClient %s disconnected on socket %i\n", connectedClient.username.c_str(), socket);

            connHandler.removeSocketFromUser(connectedClient.username, socket);
            shouldKeepExecuting = false;

            if (replicationHelper.isMainServer()) 
                for (auto mirror : replicationHelper.getMirrors())
                    serverSocket.sendPacketToClient(mirror, packet);

            break;
        }

		case LIST_REQUISITION: {
            vector <FileForListing> filesOnUserDir = fileHandler.getFiles(connectedClient.username.c_str());

            printf("\nI will send %s's file list on socket %i\n", connectedClient.username.c_str(), socket);
            serverSocket.sendFileList(socket, filesOnUserDir);

            break;
        }
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

void *handleMainServerAnswers(void *voidSocket) {
	int socket = *(int*) voidSocket;
	while(!replicationHelper.isMainServer()) {
        printf("I'm waiting for the main server on %i\n", socket);
		Packet* packet = serverSocket.receivePacketFromClient(socket);
		handleReceivedPacket(socket, packet);
	}
}

bool isMirror(int argc) {
    return argc > 3;
}

void processNewClientConnected() {
    pthread_t connectionThread;
    Connection clientConnection = serverSocket.acceptClientConnection();
    int descriptor = clientConnection.descriptor;
    pthread_create(&connectionThread, NULL, handleNewConnection, &descriptor);
}

void processMainServerAnswers() {
    pthread_t connectionThread;
    int descriptor = clientSocket.getSocketDescriptor();
    pthread_create(&connectionThread, NULL, handleMainServerAnswers, &descriptor);
    pthread_join(connectionThread, NULL);
}

void connectAsMirror(char *argv[]) {
    if (!clientSocket.setServer(string(argv[2]), stoi(string(argv[3])))) {
        printf("Error seeting server\n");
        return;
    }
    if (!clientSocket.connectToServer()) {
        printf("Error connecting to main server\n"); 
        return;
    }
    if (!clientSocket.identifyAsMirror()) {
        printf("Error sending identification\n");
        return;
    }
    printf("I will be a mirror of %s:%s\n", argv[2], argv[3]);
}

int main(int argc, char *argv[]) {

	serverSocket.listenOnPort(getServerPort(argc, argv));
    
    if (isMirror(argc)) {
        connectAsMirror(argv);
        replicationHelper.setAsBackupServer();
        fileHandler.configAsBackup();
    }

	if (!serverSocket.openSocket()) {
		printf("Could not open socket. Try another port\n");
		return -1;
	}

	serverSocket.setNumberOfClients(5);
	fileHandler.createDir();


    while (true) 
        if (replicationHelper.isMainServer()) 
            processNewClientConnected();    
        else 
            processMainServerAnswers();
    
	serverSocket.closeSocket();
	return 0;
}
