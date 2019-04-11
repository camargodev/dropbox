#include "../include/ServerSocketWrapper.hpp"

ServerSocketWrapper :: ServerSocketWrapper(int port) {
    this->socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address = this->buildAddress(port);
    bind(this->socketDescriptor, (struct sockaddr *) &address, sizeof(address));
}

sockaddr_in ServerSocketWrapper :: buildAddress(int port) {
    struct sockaddr_in address = buildDefaultAddress(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
    return address;
}

string ServerSocketWrapper :: readFromClient(SocketDescriptor clientConnectionDescriptor) {
    return readFromConnection(clientConnectionDescriptor);
}

void ServerSocketWrapper :: writeToClient(SocketDescriptor clientConnectionDescriptor, string message) {
    return writeToConnection(clientConnectionDescriptor, message);
}

Connection ServerSocketWrapper :: acceptClientConnection() {
    Connection connection;
    connection.descriptor = accept(this->socketDescriptor, (struct sockaddr *) &connection.address, &connection.lenght);
    return connection;
}

void ServerSocketWrapper :: setNumberOfClients(int numOfClients) {
    listen(this->socketDescriptor, numOfClients);
}

