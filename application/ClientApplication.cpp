#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/ClientSocketWrapper.hpp"
#include "../include/InputCommand.hpp"
#include "../include/PacketHandler.hpp"

ClientSocketWrapper clientSocket;
PacketHandler packetHandler;
SocketDescriptor serverDescriptor;

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
    if (strCommand.compare("download") == 0)
        return COMMAND_DOWNLOAD;
    if (strCommand.compare("delete") == 0)
        return COMMAND_DELETE;
    if (strCommand.compare("list_client") == 0)
        return COMMAND_LIST_CLIENT;
    if (strCommand.compare("list_server") == 0)
        return COMMAND_LIST_SERVER;
    if (strCommand.compare("get_sync_dir") == 0)
        return COMMAND_GET_SYNC_DIR;
    return INVALID_COMMAND;
}

char* getInputFilename() {
    char* filename = strtok(NULL, "\0");
    int filenameSize = strlen(filename);
    if (filename[filenameSize - 1] == '\n')
        filename[filenameSize - 1] = 0;
    return filename;
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
            command.args.fileToUpload = getInputFilename();
            break;
        case COMMAND_DOWNLOAD:
            command.args.fileToDownload = getInputFilename();
            break;
        case COMMAND_DELETE:
            command.args.fileToDelete = getInputFilename();
            break;
    }
    return command;
}

bool handleReceivedPacket(Packet* packet) {
    switch (packet->command) {
        case DOWNLOADED_FILE:
            packetHandler.addPacketToReceivedFile(serverDescriptor, packet->filename, packet);
            if (packet->currentPartIndex == packet->numberOfParts) {
                string content = packetHandler.getFileContent(serverDescriptor, packet->filename);
                printf("\nI downloaded file %s with payload:\n%s\n", packet->filename, content.c_str());
				packetHandler.removeFileFromBeingReceivedList(serverDescriptor, packet->filename);
            }
			return true;
        case SIMPLE_MESSAGE:
            printf("I received a simple message from server: %s\n", packet->payload);
            return true;
        case ERROR_MESSAGE:
            printf("Error with file %s: %s\n", packet->filename, packet->payload);
            return true;
        case DELETE_ORDER:
            printf("Server said I should delete file %s\n", packet->filename);
            return true;
    }
}

void *handleServerAnswers(void* dummy) {
    while (true) {
        Packet* packet = clientSocket.receivePacketFromServer();
        handleReceivedPacket(packet);
    }
}

int main(int argc, char *argv[])
{
    ClientInput input = getServerToConnect(argc, argv);
    
    if (!clientSocket.setServer(input.serverHostname, input.serverPort)) 
        return -1;

    if (!clientSocket.connectToServer()) {
        printf("Host %s:%i not found (is server running?)\n", argv[1], SocketWrapper::DEFAULT_PORT);
        return -1;
    }

    if (!clientSocket.identifyUsername(input.username)) {
        printf("Could not send your username\n");
    }

    serverDescriptor = clientSocket.getSocketDescriptor();

    pthread_t connectionThread;
    printf("Creating thread to get server answers...\n");
    pthread_create(&connectionThread, NULL, handleServerAnswers, NULL);

    bool shouldExit = false;
    while (!shouldExit) {
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
                break;
            case COMMAND_DOWNLOAD:
                if (!clientSocket.askToDownloadFile(command.args.fileToDownload))
                    printf("Could not download your file\n");
                break;
            case COMMAND_DELETE:
                if (!clientSocket.deleteFile(command.args.fileToDelete))
                    printf("Could not delete your file\n");
                break;
            case COMMAND_LIST_CLIENT:
                printf("List Client not implemented yet\n");
                break;
            case COMMAND_LIST_SERVER:
                printf("List Server not implemented yet\n");
                break;
            case COMMAND_GET_SYNC_DIR:
                printf("Get Sync Dir not implemented yet\n");
                break;
        }
    }

    clientSocket.closeSocket();
    
    return 0;
}
