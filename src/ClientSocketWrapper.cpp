#include "../include/ClientSocketWrapper.hpp"

ClientSocketWrapper :: ClientSocketWrapper(string serverHostname, int serverPort) {
    this->socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    hostent* host = gethostbyname(serverHostname.c_str());
    if (host != NULL) {
      struct sockaddr_in serverAddress = this->buildAddress(*((struct in_addr *)host->h_addr), serverPort);
      connect(this->socketDescriptor,(struct sockaddr *) &serverAddress, sizeof(serverAddress));
      foundHostName = true;
    } else {
      printf("incorrect host name\n");
    }
}


sockaddr_in ClientSocketWrapper :: buildAddress(in_addr hostname, int port) {
    struct sockaddr_in address = buildDefaultAddress(port);
	address.sin_addr = hostname;
    return address;
}

string ClientSocketWrapper :: readFromServer() {
    return readFromConnection(this->socketDescriptor);
}

void ClientSocketWrapper :: writeToServer(string message) {
    return writeToConnection(this->socketDescriptor, message);
}
