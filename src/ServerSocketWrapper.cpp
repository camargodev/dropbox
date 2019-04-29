#include "../include/ServerSocketWrapper.hpp"
#include "math.h"

void ServerSocketWrapper :: listenOnPort(int port) {
    this->port = port;
}

bool ServerSocketWrapper :: openSocket() {
    this->socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socketDescriptor == -1)
        return false;
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
    memset(&connection, 0, sizeof(Connection));
    connection.descriptor = accept(this->socketDescriptor, (struct sockaddr *) &connection.address, &connection.lenght);
    return connection;
}

void ServerSocketWrapper :: setNumberOfClients(int numOfClients) {
    listen(this->socketDescriptor, numOfClients);
}

bool ServerSocketWrapper :: sendFileToClient(SocketDescriptor clientConnectionDescriptor, char* filename) {
    if (sendFile(DOWNLOADED_FILE, clientConnectionDescriptor, filename)) {
        printf("File sent with success!\n");
        return true;
    }
    printf("Could not send file %s\n", filename);
    char errorMessage[PAYLOAD_SIZE] = "File not found";
    Packet packet(FILE_DOWNLOAD_ERROR, filename, errorMessage);
    return sendPacketToClient(clientConnectionDescriptor, &packet);
}

