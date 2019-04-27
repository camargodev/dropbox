#include <stdio.h>
#include <stdlib.h>
#include "../include/ClientSocketWrapper.hpp"

pair<Hostname, Port> getServerToConnect(int argc, char *argv[]) {
    if (argc < 2) {
        printf("No hostname was supplied. Please connect with ./client <server_ip> <server_port>");
        exit(EXIT_FAILURE);
    }
    int port = SocketWrapper::DEFAULT_PORT;
    if (argc == 3)
        port = stoi(string(argv[2]));
    return make_pair(argv[1], port);
}

int main(int argc, char *argv[])
{
    pair<Hostname, Port> serverIpAndPort = getServerToConnect(argc, argv);
    ClientSocketWrapper clientSocket;
    
    if (!clientSocket.setServer(serverIpAndPort.first, serverIpAndPort.second)) {
        printf("\nHost %s:%i not found (is server running?)", argv[1], SocketWrapper::DEFAULT_PORT);
        return -1;
    }

    char filename[FILENAME_SIZE] = "./input/test.txt";
    printf("I'll send %s", filename);

    if (!clientSocket.connectToServer())
        return -1;

    if (!clientSocket.uploadFileToServer(filename)) {
        printf("\nCould not send your file");
    }

    Packet* answer = clientSocket.receivePacketFromServer();
    printf("\nServer Answer is: %s\n", answer->payload);

    clientSocket.closeSocket();
    
    return 0;
}
