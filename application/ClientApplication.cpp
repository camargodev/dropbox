#include <stdio.h>
#include "../include/ClientSocketWrapper.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("No host name\n");
        return -1;
    }

    ClientSocketWrapper clientSocket;
    
    while (true) {

        if (!clientSocket.setServer(argv[1], SocketWrapper::DEFAULT_PORT)) {
            printf("\nHost %s:%i not found (is server running?)", argv[1], SocketWrapper::DEFAULT_PORT);
            return -1;
        }
        
        if (!clientSocket.connectToServer()) {
            printf("\nCould not connect to server");
            return -1;
        }

        printf("Enter the message: ");
        char input[PAYLOAD_SIZE] = "";
        fgets(input, PAYLOAD_SIZE, stdin);

        Packet packet;
        strcpy(packet.payload, input);
        if (!clientSocket.sendPacketToServer(&packet)) {
            printf("\nCould not send your message");
        }

        Packet* answer = clientSocket.receivePacketFromServer();
        printf("%s\n", answer->payload);

        clientSocket.closeSocket();
    }
    return 0;
}
