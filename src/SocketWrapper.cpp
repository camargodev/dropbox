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

string SocketWrapper :: readFromConnection(SocketDescriptor connectionDescriptor) {
	char buffer[MESSAGE_SIZE] = "";
    read(connectionDescriptor, buffer, MESSAGE_SIZE);
    return string(buffer);
}

void SocketWrapper :: writeToConnection(SocketDescriptor connectionDescriptor, string message) {
     write(connectionDescriptor, message.c_str(), message.size());
}

void SocketWrapper :: closeConnection(Connection connection) {
    close(connection.descriptor);
}

void SocketWrapper :: closeSocket() {
    close(this->socketDescriptor);
}