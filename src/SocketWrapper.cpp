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

Packet* SocketWrapper :: readFromConnection(int connectionDescriptor) {
	char* buffer = new char[sizeof(Packet)];
    read(connectionDescriptor, buffer, sizeof(Packet));
    return (Packet *) buffer;
}

void SocketWrapper :: writeToConnection(SocketDescriptor connectionDescriptor, Packet* packet) {
     write(connectionDescriptor, (void *) packet, sizeof(Packet));
}

void SocketWrapper :: closeConnection(Connection connection) {
    close(connection.descriptor);
}

void SocketWrapper :: closeSocket() {
    close(this->socketDescriptor);
}