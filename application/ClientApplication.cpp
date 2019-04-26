#include <stdio.h>
#include "../include/ClientSocketWrapper.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("No host name\n");
        return -1;
    }

    ClientSocketWrapper clientSocket;
    
    
    if (!clientSocket.setServer(argv[1], SocketWrapper::DEFAULT_PORT)) {
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
