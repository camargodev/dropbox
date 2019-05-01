#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/ClientSocketWrapper.hpp"
#include "../include/InputHandler.hpp"
#include "../include/PacketHandler.hpp"

ClientSocketWrapper clientSocket;
PacketHandler packetHandler;
SocketDescriptor serverDescriptor;
FileHandler fileHandler;

vector<FileForListing> receivedFileList;

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
        return INPUT_UPLOAD;
    if (strCommand.compare("exit") == 0)
        return INPUT_EXIT;
    if (strCommand.compare("download") == 0)
        return INPUT_DOWNLOAD;
    if (strCommand.compare("delete") == 0)
        return INPUT_DELETE;
    if (strCommand.compare("list_client") == 0)
        return INPUT_LIST_CLIENT;
    if (strCommand.compare("list_server") == 0)
        return INPUT_LIST_SERVER;
    if (strCommand.compare("get_sync_dir") == 0)
        return INPUT_GET_SYNC_DIR;
    return INVALID_INPUT;
}

char* getNextValueOnInput() {
    char* filename = strtok(NULL, "\0");
    int filenameSize = strlen(filename);
    if (filename[filenameSize - 1] == '\n')
        filename[filenameSize - 1] = 0;
    return filename;
}

Input proccesCommand(char userInput[INPUT_SIZE]) {
    char *inputOperation = strtok(userInput, " ");
    int inputOperationSize = strlen(inputOperation);
    if (inputOperation[inputOperationSize - 1] == '\n')
        inputOperation[inputOperationSize - 1] = 0;
    int inputCode = getCommandCode(inputOperation);
    Input input(inputCode);
    switch (inputCode) {
        case INPUT_UPLOAD:
            input.args.fileToUpload = getNextValueOnInput();
            break;
        case INPUT_DOWNLOAD:
            input.args.fileToDownload = getNextValueOnInput();
            break;
        case INPUT_DELETE:
            input.args.fileToDelete = getNextValueOnInput();
            break;
        case INPUT_LIST_CLIENT:
            input.args.directory = fileHandler.getLocalDirectoryName();
            break;
    }
    return input;
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
        case FILE_LISTING:
            FileForListing receivedFile(packet->filename);
            receivedFile.modificationTime = packet->modificationTime;
            receivedFile.accessTime = packet->accessTime;
            receivedFile.creationTime = packet->creationTime;
            receivedFileList.push_back(receivedFile);
            if (packet->currentPartIndex == packet->numberOfParts) {
                fileHandler.printFileList(receivedFileList);
                receivedFileList.clear();
            }

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
        char inputCommand[INPUT_SIZE] = "";
        fgets(inputCommand, INPUT_SIZE, stdin);
        Input input = proccesCommand(inputCommand);
        switch (input.inputCode) {
            case INPUT_EXIT:
                clientSocket.disconnectFromServer();
                shouldExit = true;
                break;
            case INPUT_UPLOAD:
                if (!clientSocket.uploadFileToServer(input.args.fileToUpload))
                    printf("Could not send your file\n");
                break;
            case INPUT_DOWNLOAD:
                if (!clientSocket.askToDownloadFile(input.args.fileToDownload))
                    printf("Could not download your file\n");
                break;
            case INPUT_DELETE:
                if (!clientSocket.deleteFile(input.args.fileToDelete))
                    printf("Could not delete your file\n");
                break;
            case INPUT_LIST_CLIENT:
                fileHandler.printFileList(fileHandler.getFilesInDir(input.args.directory));
                break;
            case INPUT_LIST_SERVER:
                clientSocket.askForFileList();
                break;
            case INPUT_GET_SYNC_DIR:
                printf("Get Sync Dir not implemented yet\n");
                break;
        }
    }

    clientSocket.closeSocket();
    
    return 0;
}
