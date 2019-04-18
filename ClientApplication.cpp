#include <stdio.h>
#include "include/ClientSocketWrapper.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("no host name");
        return -1;
    }

    ClientSocketWrapper clientSocket(argv[1], SocketWrapper::DEFAULT_PORT);
    
    printf("Enter the message: ");
    char input[256] = "";
    fgets(input, 256, stdin);

    clientSocket.writeToServer(string(input));

    string answer = clientSocket.readFromServer();
    printf("%s\n", answer.c_str());
    
	clientSocket.closeSocket();
    return 0;
}