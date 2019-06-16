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
    return ::bind(this->socketDescriptor, (struct sockaddr *) &address, sizeof(address)) >= 0;
}

sockaddr_in ServerSocketWrapper :: buildAddress(int port) {
    struct sockaddr_in address = buildDefaultAddress(port);
	address.sin_addr.s_addr = INADDR_ANY;
    return address;
}

Packet* ServerSocketWrapper :: receivePacketFromClient(SocketDescriptor clientConnectionDescriptor) {
    return this->receivePacketFromClient(clientConnectionDescriptor, 0);
}

Packet* ServerSocketWrapper :: receivePacketFromClient(SocketDescriptor clientConnectionDescriptor, int timeout) {
    Packet* packet = receivePacket(clientConnectionDescriptor, timeout);
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

bool ServerSocketWrapper :: sendSyncFile(SocketDescriptor clientConnectionDescriptor, WrappedFile file) {
    return sendFileToClient(FILE_SYNCED, clientConnectionDescriptor, file);
}

bool ServerSocketWrapper :: sendDownloadedFile(SocketDescriptor clientConnectionDescriptor, WrappedFile file) {
    return sendFileToClient(DOWNLOADED_FILE, clientConnectionDescriptor, file);
}

bool ServerSocketWrapper :: sendFileToClient(Command command, SocketDescriptor clientConnectionDescriptor, WrappedFile file) {
    if (!file.isFound) {
        printf("Could not find file %s\n", file.filename);
        char errorMessage[PAYLOAD_SIZE] = "File not found";
        Packet packet(ERROR_MESSAGE, file.filename, errorMessage);
        return false;
    }
    if (!sendFile(command, clientConnectionDescriptor, file)) {
        printf("Error sending file %s\n", file.filename);
        return false;
    }
    printf("File sent with success to connection %i!\n", clientConnectionDescriptor);
    return true;
}

bool ServerSocketWrapper :: sendMirror(SocketDescriptor socket, Mirror mirror) {
    Packet packet(MIRROR_REPLICATION);
    strcpy(packet.ip, mirror.ip);
    packet.port = mirror.port;
    return this->sendPacketToClient(socket, &packet);
};

int ServerSocketWrapper :: getPort() {
    return this->port;
}

