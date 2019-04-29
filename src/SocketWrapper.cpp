#include "../include/SocketWrapper.hpp"
#include "math.h"

int calculateNumberOfPayloads(const char* filename);

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

bool SocketWrapper :: sendFile(int command, SocketDescriptor connectionDescriptor, char* filename) {
    File* file = fopen(filename, "r");
    if (file == NULL) 
        return false;
    char currentPayload[PAYLOAD_SIZE] = "";
    int numberOfReadBytes = 0;
    int currentIndex = 1;
    int numberOfParts = calculateNumberOfPayloads(filename);
    while ((numberOfReadBytes = fread(currentPayload, sizeof(char), PAYLOAD_SIZE, file)) > 0) {
        Packet packet(filename, currentIndex, numberOfParts, numberOfReadBytes, currentPayload);
        packet.command = command;
        if (!sendPacket(connectionDescriptor, &packet))
            return false;
        currentIndex++;
        memset(currentPayload, 0, PAYLOAD_SIZE);
    }
    return true;
}

std::ifstream::pos_type getFilesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

int calculateNumberOfPayloads(const char* filename) {
    int filesize = getFilesize(filename);
    return ceil((float) filesize/PAYLOAD_SIZE);
}

void SocketWrapper :: closeConnection(Connection connection) {
    close(connection.descriptor);
}

void SocketWrapper :: closeSocket() {
    close(this->socketDescriptor);
}