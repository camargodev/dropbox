#include <stdio.h>
#include <stdlib.h>
#include "../include/ClientSocketWrapper.hpp"

struct ClientInput{
    char* username;
    char* serverHostname;
    int serverPort;

    ClientInput(char* username,
                char* serverHostname,
                int serverPort) {
        this->username = username;
        this->serverHostname = serverHostname;
        this->serverPort = serverPort;
    }
};

ClientInput getServerToConnect(int argc, char *argv[]) {
    if (argc < 4) {
        printf("No hostname was supplied. Please connect with ./client <username> <server_ip> <server_port>");
        exit(EXIT_FAILURE);
    }
    int port = SocketWrapper::DEFAULT_PORT;
    if (argc == 4)
        port = stoi(string(argv[3]));
    return ClientInput(argv[1], argv[2], port);
}

int main(int argc, char *argv[])
{
    ClientInput input = getServerToConnect(argc, argv);
    ClientSocketWrapper clientSocket;
    
    if (!clientSocket.setServer(input.serverHostname, input.serverPort)) {
        printf("\nHost %s:%i not found (is server running?)", argv[1], SocketWrapper::DEFAULT_PORT);
        return -1;
    }

    if (!clientSocket.connectToServer())
        return -1;

    char filename[FILENAME_SIZE] = "./input/test.txt";
    printf("I'll send %s", filename);

    if (!clientSocket.identifyUsername(input.username)) {
        printf("\nCould not send your username");
    }

    if (!clientSocket.uploadFileToServer(filename)) {
        printf("\nCould not send your file");
    }

    Packet* answer = clientSocket.receivePacketFromServer();
    printf("\nServer Answer is: %s\n", answer->payload);

    clientSocket.closeSocket();
    
    return 0;
}
