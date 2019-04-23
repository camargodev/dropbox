#include <stdio.h>
#include "include/ClientSocketWrapper.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("no host name\n");
        return -1;
    }

    ClientSocketWrapper clientSocket(argv[1], SocketWrapper::DEFAULT_PORT);

    if (clientSocket.foundHostName == true) {
        printf("Enter the message: ");
        char input[PAYLOAD_SIZE] = "";
        fgets(input, PAYLOAD_SIZE, stdin);


		Packet packet;
		strcpy(packet.payload, input);
        clientSocket.sendPacketToServer(&packet);

        Packet* answer = clientSocket.receivePacketFromServer();
        printf("%s\n", answer->payload);

        clientSocket.closeSocket();
    } else {
       printf("Cannot connect\n");
       return -1;
    }

    return 0;
}
