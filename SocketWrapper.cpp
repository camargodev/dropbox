#include "SocketWrapper.hpp"

SocketWrapper :: SocketWrapper() {
    this->socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
}

void SocketWrapper :: configureAsServer(int port) {
    sockaddr_in address = buildDefaultAddress(port);
    bind(this->socketDescriptor, (struct sockaddr *) &address, sizeof(address));
}

void SocketWrapper :: configureAsClient(string hostname, int port) {
    hostent* host = gethostbyname(hostname.c_str());
    struct sockaddr_in serverAddress = buildAddress(*((struct in_addr *)host->h_addr), port);
    connect(this->socketDescriptor,(struct sockaddr *) &serverAddress, sizeof(serverAddress));
}

SocketDescriptor SocketWrapper :: getSocketDescriptor() {
    return this->socketDescriptor;
}

sockaddr_in SocketWrapper :: buildAddress(int port) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
	address.sin_port = htons(port);
	bzero(&(address.sin_zero), 8); 
    return address;
}

sockaddr_in SocketWrapper :: buildDefaultAddress(int port) {
    struct sockaddr_in address = buildAddress(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
    return address;
}

sockaddr_in SocketWrapper :: buildAddress(in_addr hostname, int port) {
    struct sockaddr_in address = buildAddress(port);
	address.sin_addr = hostname;
    return address;
}

Connection SocketWrapper :: acceptConnection() {
    Connection connection;
    connection.descriptor = accept(this->socketDescriptor, (struct sockaddr *) &connection.address, &connection.lenght);
    return connection;
}

void SocketWrapper :: setNumberOfClients(int numOfClients) {
    listen(this->socketDescriptor, numOfClients);
}

string SocketWrapper :: readFromClient(Connection connection) {
	char buffer[MESSAGE_SIZE] = "";
    read(connection.descriptor, buffer, MESSAGE_SIZE);
    return string(buffer);
}

void SocketWrapper :: writeToClient(Connection connection, string message) {
     write(connection.descriptor, message.c_str(), message.size());
}

string SocketWrapper :: readFromServer() {
	char buffer[MESSAGE_SIZE] = "";
    read(this->socketDescriptor, buffer, MESSAGE_SIZE);
    return string(buffer);
}

void SocketWrapper :: writeToServer(string message) {
     write(this->socketDescriptor, message.c_str(), message.size());
}

void SocketWrapper :: closeConnection(Connection connection) {
    close(connection.descriptor);
}

void SocketWrapper :: closeSocket() {
    close(this->socketDescriptor);
}