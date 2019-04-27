#include <stdio.h>
#include <stdlib.h>
#include "../include/ClientSocketWrapper.hpp"
#include "../include/InputCommand.hpp"

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

    if (!clientSocket.identifyUsername(input.username)) {
        printf("\nCould not send your username");
    }

    bool shouldExit = false;
    while (!shouldExit) {
        printf("\n> ");
        char userCommand[COMMAND_SIZE] = "";
        fgets(userCommand, COMMAND_SIZE, stdin);
        Command command = proccesCommand(userCommand);
        switch (command.commandCode) {
            case COMMAND_EXIT:
                shouldExit = true;
                break;
            case COMMAND_UPLOAD:
                if (!clientSocket.uploadFileToServer(command.args.fileToUpload))
                    printf("\nCould not send your file");
        }
    }

    // char filename[FILENAME_SIZE] = "/home/camargo/Documents/dropbox/input/test.txt";
    // printf("I'll send %s", filename);


    // char commandName[COMMAND_SIZE];
    // strcpy(commandName, "upload ./input/test.txt");
    // Command comm = proccesCommand(commandName);
    // printf("\ncode = %i file = %s", comm.commandCode, comm.args.fileToUpload);

    // if (!clientSocket.uploadFileToServer(filename)) {
    //     printf("\nCould not send your file");
    // }

    // Packet* answer = clientSocket.receivePacketFromServer();
    // printf("\nServer Answer is: %s\n", answer->payload);

    clientSocket.closeSocket();
    
    return 0;
}
