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
	int amountOfBytes = sizeof(Packet);
    void *buffer = (void*) malloc(amountOfBytes);
    int totalReadBytes = 0, bytesToRead, partialReadBytes;
    while ((bytesToRead = amountOfBytes - totalReadBytes) != 0) {
        partialReadBytes = read(socketDescriptor, (buffer + totalReadBytes), bytesToRead);
        if (partialReadBytes < 0) {
            printf("Error on reading\n");
        }
        totalReadBytes += partialReadBytes;
    }
    return (Packet*) buffer;

    // void* packet = (void *) malloc(sizeof(Packet));
	// size_t packetSize = sizeof(Packet);

    // int readBytes = 0;
    // int remainBytes = packetSize;
    // int totalBytesRead = 0;

    // while(remainBytes > 0) {
    //     readBytes = recv(connectionDescriptor, (packet + totalBytesRead), packetSize, 0);
    //     totalBytesRead += readBytes;
    //     remainBytes = packetSize - totalBytesRead;
    // }

    // return (Packet *) packet;
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
    File* file = fopen(wrappedFile.filepath.c_str(), "r");
    if(file == NULL) return false;

    size_t bytesRead;
    int numberOfParts = calculateNumberOfPayloads(wrappedFile.filesize);
    
    Packet currentPacket;
    strcpy(currentPacket.filename, wrappedFile.filename);
    currentPacket.numberOfParts = numberOfParts;

    for(int i = 0; i < numberOfParts; i++) {
        bytesRead = fread(&(currentPacket.payload), sizeof(char), PAYLOAD_SIZE, file);

        currentPacket.command = command;
        currentPacket.currentPartIndex = i + 1;
        currentPacket.payloadSize = (int) bytesRead;

        if (!sendPacket(connectionDescriptor, &currentPacket))
            return false;
    }

    fclose(file);
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