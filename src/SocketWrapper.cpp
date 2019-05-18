#include "../include/SocketWrapper.hpp"
#include "math.h"
#include <algorithm>

SocketDescriptor SocketWrapper :: getSocketDescriptor() {
    return this->socketDescriptor;
}

sockaddr_in SocketWrapper :: buildDefaultAddress(int port) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
	address.sin_port = htons(port);
	bzero(&(address.sin_zero), 8); 
    return address;
}

Packet* SocketWrapper :: receivePacket(int connectionDescriptor) {
	Packet* packet = (Packet*) malloc(sizeof(Packet));
    int readBytes = recv(connectionDescriptor, packet, sizeof(Packet), 0);
    return packet;
}

bool SocketWrapper :: sendPacket(SocketDescriptor connectionDescriptor, Packet* packet) {
    int response = send(connectionDescriptor, (void *) packet, sizeof(Packet), 0);
    if (response < 0)
        printf("Error on send is %i\n", response);
    return response  >= 0;
}

int calculateNumberOfPayloads(int filesize) {
    return ceil((float) filesize / PAYLOAD_SIZE);
}

bool SocketWrapper :: sendFile(int command, SocketDescriptor connectionDescriptor, WrappedFile wrappedFile) {
    char currentPayload[PAYLOAD_SIZE] = "";
    int numberOfReadBytes = 0;
    int currentIndex = 1;
    int numberOfParts = calculateNumberOfPayloads(wrappedFile.content.length());

    string contentPart;
    Packet currentPacket;
    strcpy(currentPacket.filename, wrappedFile.filename);
    currentPacket.numberOfParts = numberOfParts;

    for(int i = 0; i < numberOfParts; i++) {
        contentPart = wrappedFile.content.substr(i * PAYLOAD_SIZE, PAYLOAD_SIZE);

        strcpy(currentPacket.payload, contentPart.c_str());
        currentPacket.command = command;
        currentPacket.currentPartIndex = i + 1;
        currentPacket.payloadSize = contentPart.size();

        if (!sendPacket(connectionDescriptor, &currentPacket))
            return false;

        memset(currentPayload, 0, PAYLOAD_SIZE);
    }

    return true;
}

bool SocketWrapper :: sendFileList(SocketDescriptor connectionDescriptor, vector<FileForListing> files) {
    int numberOfFiles = files.size();
    int currentFileIndex = INITIAL_PART;
    for (auto file : files) {
        Packet packet(FILE_LISTING, file.filename, currentFileIndex++, numberOfFiles, 
            file.modificationTime, file.accessTime, file.creationTime);
        if (!sendPacket(connectionDescriptor, &packet))
            return false;
    }
    return true;
}

bool SocketWrapper :: sendFileListForSyncDir(SocketDescriptor connectionDescriptor, vector<FileForListing> files) {
    int numberOfFiles = files.size();
    int currentFileIndex = INITIAL_PART;
    for (auto file : files) {
        Packet packet(SYNC_FILE, file.filename, currentFileIndex++, numberOfFiles, 
            file.modificationTime, file.accessTime, file.creationTime);
        if (!sendPacket(connectionDescriptor, &packet))
            return false;
    }
    return true;
}

int SocketWrapper :: getNumberOfPayloadsForFile(int fileContentSize) {
    return ceil((float) fileContentSize/PAYLOAD_SIZE);
}

void SocketWrapper :: closeConnection(Connection connection) {
    close(connection.descriptor);
}

void SocketWrapper :: closeSocket() {
    close(this->socketDescriptor);
}