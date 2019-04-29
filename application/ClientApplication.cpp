#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/ClientSocketWrapper.hpp"
#include "../include/InputCommand.hpp"

ClientSocketWrapper clientSocket;

ClientInput getServerToConnect(int argc, char *argv[]) {
    if (argc < 4) {
        printf("No hostname was supplied. Please connect with ./client <username> <server_ip> <server_port>\n");
        exit(EXIT_FAILURE);
    }
    int port = SocketWrapper::DEFAULT_PORT;
    if (argc == 4)
        port = stoi(string(argv[3]));
    return ClientInput(argv[1], argv[2], port);
}

int getCommandCode(char* commandName) {
    string strCommand = string(commandName);
    if (strCommand.compare("upload") == 0)
        return COMMAND_UPLOAD;
    if (strCommand.compare("exit") == 0)
        return COMMAND_EXIT;
    return INVALID_COMMAND;
}

Command proccesCommand(char userCommand[COMMAND_SIZE]) {
    char *commandName = strtok(userCommand, " ");
    int commandNameSize = strlen(commandName);
    if (commandName[commandNameSize - 1] == '\n')
        commandName[commandNameSize - 1] = 0;
    int commandCode = getCommandCode(commandName);
    Command command(commandCode);
    switch (commandCode) {
        case COMMAND_UPLOAD:
            char* filename = strtok(NULL, "\0");
            int filenameSize = strlen(filename);
            if (filename[filenameSize - 1] == '\n')
                filename[filenameSize - 1] = 0;
            command.args.fileToUpload = filename;
            break;
    }
    return command;
}

void *handleServerAnswers(void* dummy) {
    while (true) {
        Packet* packet = clientSocket.receivePacketFromServer();
        printf("SERVER sent: %s\n", packet->payload);
    }
}

int main(int argc, char *argv[])
{
    ClientInput input = getServerToConnect(argc, argv);
    
    if (!clientSocket.setServer(input.serverHostname, input.serverPort)) {
        printf("Host %s:%i not found (is server running?)\n", argv[1], SocketWrapper::DEFAULT_PORT);
        return -1;
    }

    if (!clientSocket.connectToServer())
        return -1;

    if (!clientSocket.identifyUsername(input.username)) {
        printf("Could not send your username\n");
    }

    pthread_t connectionThread;
    printf("Creating thread to get server answers...\n");
    pthread_create(&connectionThread, NULL, handleServerAnswers, NULL);

    bool shouldExit = false;
    while (!shouldExit) {
        printf("> ");
        char userCommand[COMMAND_SIZE] = "";
        fgets(userCommand, COMMAND_SIZE, stdin);
        Command command = proccesCommand(userCommand);
        switch (command.commandCode) {
            case COMMAND_EXIT:
                clientSocket.disconnectFromServer();
                shouldExit = true;
                break;
            case COMMAND_UPLOAD:
                if (!clientSocket.uploadFileToServer(command.args.fileToUpload))
                    printf("Could not send your file\n");
        }
    }

    clientSocket.closeSocket();
    
    return 0;
}
