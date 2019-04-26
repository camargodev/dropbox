#include "../include/SocketWrapper.hpp"

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
	char* buffer = new char[sizeof(Packet)];
    strcpy(buffer, "");
    int readBytes = recv(connectionDescriptor, buffer, sizeof(Packet), 0);
    printf("\nI read %i bytes", readBytes);
    return (Packet *) buffer;
}

bool SocketWrapper :: sendPacket(SocketDescriptor connectionDescriptor, Packet* packet) {
    int response = send(connectionDescriptor, (void *) packet, sizeof(Packet), 0);
    if (response < 0)
        printf("\nError on send is %i", response);
    return response  >= 0;
}

void SocketWrapper :: closeConnection(Connection connection) {
    close(connection.descriptor);
}

void SocketWrapper :: closeSocket() {
    close(this->socketDescriptor);
}