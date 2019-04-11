#include "SocketWrapper.hpp"


SocketWrapper :: SocketWrapper(int port) {
    this->socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address = buildAddress(port);
    bind(this->socketDescriptor, (struct sockaddr *) &address, sizeof(address));
}

SocketDescriptor SocketWrapper :: getSocketDescriptor() {
    return this->socketDescriptor;
}

sockaddr_in SocketWrapper :: buildAddress(int port) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;
	bzero(&(address.sin_zero), 8); 
    return address;
}

SocketDescriptor SocketWrapper :: acceptConnection() {
    struct sockaddr_in clientAddress;
	socklen_t clientLenght;
    return accept(this->socketDescriptor, (struct sockaddr *) &clientAddress, &clientLenght);
}

void SocketWrapper :: setNumberOfClients(int numOfClients) {
    listen(this->socketDescriptor, numOfClients);
}

string SocketWrapper :: readFromConnection(SocketDescriptor connection) {
	char buffer[MESSAGE_SIZE];
    read(connection, buffer, MESSAGE_SIZE);
    return string(buffer);
}

void SocketWrapper :: writeToConnection(SocketDescriptor connection, string message) {
     write(connection, message.c_str(), message.size());
}

void SocketWrapper :: closeConnection(SocketDescriptor connection) {
    close(connection);
}