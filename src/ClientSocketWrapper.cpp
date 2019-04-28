#include "../include/ClientSocketWrapper.hpp"
#include "math.h"

int calculateNumberOfPayloads(const char* filename);

bool ClientSocketWrapper :: setServer(string hostname, int port) {
    this->socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    hostent* host = gethostbyname(hostname.c_str());
    if (host != NULL) {
        this->serverAddress = this->buildAddress(*((struct in_addr *)host->h_addr), port);
        return true;
    }
    return false;
}

bool ClientSocketWrapper :: connectToServer() {
    return connect(this->socketDescriptor,(struct sockaddr *) &this->serverAddress, sizeof(this->serverAddress)) >= 0;
}

sockaddr_in ClientSocketWrapper :: buildAddress(in_addr hostname, int port) {
    struct sockaddr_in address = buildDefaultAddress(port);
	address.sin_addr = hostname;
    return address;
}

Packet* ClientSocketWrapper :: receivePacketFromServer() {
    return receivePacket(this->socketDescriptor);
}

bool ClientSocketWrapper :: sendPacketToServer(Packet* packet) {
    return sendPacket(this->socketDescriptor, packet);
}

bool ClientSocketWrapper :: identifyUsername(char* username) {
    Packet* packet = new Packet;
    packet->command = IDENTIFICATION;
    strcpy(packet->payload, username);
    packet->payloadSize = sizeof(username);
    strcpy(packet->filename, "");
    packet->currentPartIndex = 1;
    packet->numberOfParts = 1;
    return sendPacketToServer(packet);
}

bool ClientSocketWrapper :: uploadFileToServer(char* filename) {
    return sendFile(this->socketDescriptor, filename);
}
