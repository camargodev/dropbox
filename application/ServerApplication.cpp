#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include "../include/ServerSocketWrapper.hpp"
#include "../include/PacketHandler.hpp"
#include "../include/ConnectionHandler.hpp"
#include "../include/ServerFileHandler.hpp"

PacketHandler packetHandler;
ConnectionHandler connHandler;
ServerSocketWrapper serverSocket;
ServerFileHandler fileHandler;

int getServerPort(int argc, char *argv[]) {
	int port = SocketWrapper::DEFAULT_PORT;
	if (argc == 2)
        port = stoi(string(argv[1]));
	return port;
}

bool receivedFromTheCurrentOpenSocket(SocketDescriptor originSocket, SocketDescriptor openSocket) {
	return originSocket == openSocket;
}

string getCorrectFilename(const string& s, string username) { 
	char* home = getenv("HOME");    
   char sep = '/';
   size_t i = s.rfind(sep, s.length());
   if (i != string::npos) {
      string f = s.substr(i+1, s.length() - i);
	  char filename[300];  
	  ::sprintf(filename, "%s/server_dir/sync_dir_%s/%s", home, username.c_str(), f.c_str()); 
	 return filename;
   }
   return("");
}

void handleUploadedFile(string username, Packet* packet) {
	// packetHandler.addPacketToReceivedFile(socket, packet->filename, packet);
	string filenameToSave = getCorrectFilename(packet->filename, username);
	File* file = fopen(filenameToSave.c_str(), "a"); 
	fwrite(&(packet->payload), 1, packet->payloadSize, file);
	fclose(file);
}

bool handleReceivedPacket(int socket, Packet* packet) {
    ConnectedClient connectedClient = connHandler.getConnectedClientBySocket(socket);
	bool shouldKeepExecuting = true;
	string filenameToSave;

	switch (packet->command) {
		case GET_SYNC_DIR: {
            printf("Client %s requested sync dir\n", connectedClient.username.c_str());
            vector <FileForListing> filesOnUserDirectory = fileHandler.getFiles(connectedClient.username.c_str());
            printf("\nI will send %s's file list on socket %i\n", connectedClient.username.c_str(), socket);
            serverSocket.sendFileListForSyncDir(socket, filesOnUserDirectory);
        }
			break;

        case UPLOAD_FILE: {
            handleUploadedFile(connectedClient.username, packet);
            packetHandler.addPacketToReceivedFile(socket, packet->filename, packet);
            // filenameToSave = getCorrectFilename(packet->filename, connectedClient.username);
            // printf("FILENAME IS %s\n", filenameToSave.c_str());
            // File* file = fopen(filenameToSave.c_str(), "a");
            // fwrite(&(packet->payload), 1, packet->payloadSize, file);
            // fclose(file);
            if (packet->currentPartIndex == packet->numberOfParts) {
                // string content = packetHandler.getFileContent(socket, packet->filename);
                // int contentSize = packetHandler.getFileContentSize(socket, packet->filename);
                // printf("\nI received file %s\n", packet->filename);
                // connectedClient = connHandler.getConnectedClientBySocket(socket);
                // printf("Now I will notify user %s\n", connectedClient.username.c_str());
                for (auto openSocket : connectedClient.openSockets) {
                    // if (!receivedFromTheCurrentOpenSocket(socket, openSocket)) {
                    WrappedFile file = fileHandler.getFile(connectedClient.username.c_str(), packet->filename);
                    serverSocket.sendFileToClient(openSocket, file);
                    // }
                }
                // // fileHandler.createFile(connectedClient.username.c_str(), packet->filename, content, contentSize);
                // string filenameOk = getCorrectFilename(packet->filename, connectedClient.username);
                // File* file = fopen(filenameOk.c_str(), "w"); 
                // printf("FILENAME IS %s\n", filenameOk.c_str());
                // fwrite(&(content), 1, contentSize, file);
                // fclose(file);
                packetHandler.removeFileFromBeingReceivedList(socket, packet->filename);
            }
        }
			break;

		case DOWNLOAD_REQUISITION: {
            printf("\nI'll try to send file %s to client of socket %i\n", packet->payload, socket);

            WrappedFile file = fileHandler.getFile(connectedClient.username.c_str(), packet->payload);
            serverSocket.sendFileToClient(socket, file);
        }
			break;

		case DELETE_REQUISITION: {
            fileHandler.deleteFile(connectedClient.username.c_str(), packet->filename);
            for (auto openSocket : connectedClient.openSockets) {
                Packet answer(DELETE_ORDER, packet->filename);
                serverSocket.sendPacketToClient(openSocket, &answer);
            }
        }
			break;

		case IDENTIFICATION: {
            printf("\nClient %s connected on socket %i\n", packet->payload, socket);
            connHandler.addSocketToClient(packet->payload, socket);
        }
            break;

		case DISCONNECT: {
            printf("\nClient %s disconnected on socket %i\n", connectedClient.username.c_str(), socket);
            connHandler.removeSocketFromUser(connectedClient.username, socket);
            shouldKeepExecuting = false;
        }
			break;

		case LIST_REQUISITION: {
            vector <FileForListing> filesOnUserDir = fileHandler.getFiles(connectedClient.username.c_str());
            printf("\nI will send %s's file list on socket %i\n", connectedClient.username.c_str(), socket);
            serverSocket.sendFileList(socket, filesOnUserDir);
        }
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
	fileHandler.createDir();

	while (true) {

		pthread_t connectionThread;
		Connection clientConnection = serverSocket.acceptClientConnection();
		int descriptor = clientConnection.descriptor;
		pthread_create(&connectionThread, NULL, handleNewConnection, &descriptor);

	}

	serverSocket.closeSocket();
	return 0;
}
