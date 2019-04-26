#include "../include/ClientSocketWrapper.hpp"
#include "math.h"

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
