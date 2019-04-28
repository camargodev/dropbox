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
    File* file = fopen(filename, "r");
    if (file == NULL) 
        return false;
    char currentPayload[PAYLOAD_SIZE] = "";
    int numberOfReadBytes = 0;
    int currentIndex = 1;
    int numberOfParts = calculateNumberOfPayloads(filename);
    while ((numberOfReadBytes = fread(currentPayload, sizeof(char), PAYLOAD_SIZE, file)) > 0) {
        Packet packet(filename, currentIndex, numberOfParts, numberOfReadBytes, currentPayload);
        packet.command = UPLOAD_FILE;
        if (!sendPacketToServer(&packet))
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
