#include "../include/ServerSocketWrapper.hpp"
#include "math.h"

ServerSocketWrapper :: ServerSocketWrapper(int port) {
    this->port = port;
}

bool ServerSocketWrapper :: openSocket() {
    this->socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socketDescriptor == -1) {
        printf("\nHere is the scoop");
        return false;
    }
    sockaddr_in address = this->buildAddress(port);
    return bind(this->socketDescriptor, (struct sockaddr *) &address, sizeof(address)) >= 0;
}

sockaddr_in ServerSocketWrapper :: buildAddress(int port) {
    struct sockaddr_in address = buildDefaultAddress(port);
	address.sin_addr.s_addr = INADDR_ANY;
    return address;
}

Packet* ServerSocketWrapper :: receivePacketFromClient(SocketDescriptor clientConnectionDescriptor) {
    Packet* packet = receivePacket(clientConnectionDescriptor);
    return packet;
}

bool ServerSocketWrapper :: sendPacketToClient(SocketDescriptor clientConnectionDescriptor, Packet* packet) {
    return sendPacket(clientConnectionDescriptor, packet);
}

Connection ServerSocketWrapper :: acceptClientConnection() {
    Connection connection;
    connection.descriptor = accept(this->socketDescriptor, (struct sockaddr *) &connection.address, &connection.lenght);
    return connection;
}

void ServerSocketWrapper :: setNumberOfClients(int numOfClients) {
    listen(this->socketDescriptor, numOfClients);
}

void ServerSocketWrapper :: sendFileToClient(SocketDescriptor clientConnectionDescriptor, File* file) {
}

