#include <sys/inotify.h>
#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <algorithm>

#include "../include/ClientSocketWrapper.hpp"
#include "../include/ClientSyncWrapper.hpp"
#include "../include/InputHandler.hpp"
#include "../include/PacketHandler.hpp"
#include "../include/Notifier.hpp"

vector<FileForListing> receivedFileList;
SocketDescriptor serverDescriptor;
ClientSocketWrapper clientSocket;
PacketHandler packetHandler;
ClientFileHandler fileHandler;
vector<string> filesBeingReceived;
char* clientUsername;
Notifier notifier;
InputHandler inputHandler;

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

void handleReceivedPacket(Packet* packet) {
    switch (packet->command) {
        case SYNC_FILE:
            printf("I must download file %s\n", packet->filename);
            if (!clientSocket.getFileFromSyncDir(packet->filename))
                printf("Could not download your file\n");
            break;
        case DOWNLOADED_FILE: {
            string filepath = fileHandler.getDownloadFilepath(packet->filename);

            if(packet->currentPartIndex == 1) {
                fileHandler.createFile(filepath.c_str(), packet->payload, packet->payloadSize);
            } else {
                fileHandler.appendFile(filepath.c_str(), packet->payload, packet->payloadSize);
            }
            if (packet->currentPartIndex == packet->numberOfParts) {
                printf("Finished downloading file %s with %i packets\n", packet->filename, packet->numberOfParts);
            }
            break;
        }
        case FILE_SYNCED: {
            string filepath = fileHandler.getFilepath(packet->filename);

            if(packet->currentPartIndex == 1) {
                notifier.stopWatching();
                fileHandler.createFile(filepath.c_str(), packet->payload, packet->payloadSize);
            } else {
                fileHandler.appendFile(filepath.c_str(), packet->payload, packet->payloadSize);
            }
            if (packet->currentPartIndex == packet->numberOfParts) {
                printf("Finished getting synched file %s with %i packets\n", packet->filename, packet->numberOfParts);
                notifier.startWatching();
            }
            break;
        }
        case SIMPLE_MESSAGE:
            printf("I received a simple message from server: %s\n", packet->payload);
            break;
        case ERROR_MESSAGE:
            printf("Error with file %s: %s\n", packet->filename, packet->payload);
            break;
        case DELETE_ORDER: {
            string filepath = fileHandler.getFilepath(packet->filename);
            fileHandler.deleteFile(filepath.c_str());
            break;
        }
        case FILE_LISTING: {
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
            break;
    }
}

void *handleServerAnswers(void* dummy) {
    while (true) {
        Packet* packet = clientSocket.receivePacketFromServer();
        handleReceivedPacket(packet);
    }
}

void handleFileUpdate(const char* filename) {
    WrappedFile file = fileHandler.getFile(filename);
    if(!clientSocket.uploadFileToServer(file))
        printf("Notify >> Could not upload your file\n");
}

void handleFileDeletion(char* filename) {
    if(!clientSocket.deleteFile(filename))
        printf("Notify >> Could not delete your file\n");
}

bool isFileBeingReceived(string filename) {
    return find(filesBeingReceived.begin(), filesBeingReceived.end(), filename) != filesBeingReceived.end();
}

void *handleNotifyEvents(void* dummy) {
    notifier.setDirectory(fileHandler.getDirpath());
    notifier.startWatching();
    while(true) {
        Action action = notifier.getListenedAction();
        if (action.type == Notifier::NO_ACTION || isFileBeingReceived(action.filename))
            continue;
        char actionFilename[FILENAME_SIZE];
        string fullFilename = fileHandler.getFilepath(action.filename.c_str());
        strcpy(actionFilename, fullFilename.c_str());
        switch(action.type) {
            case Notifier::CREATE:
                printf("I'll upload %s\n", actionFilename);
                handleFileUpdate(actionFilename);
                break;
            case Notifier::EDIT:
                handleFileUpdate(actionFilename);
                break;
            case Notifier::DELETE:
                printf("I'll ask to delete %s\n", actionFilename);
                handleFileDeletion(actionFilename);
                break;
            default: 
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    ClientInput input = getServerToConnect(argc, argv);
    
    if (!clientSocket.setServer(input.serverHostname, input.serverPort)) 
        return -1;

    if (!clientSocket.connectToServer()) {
        printf("Host %s:%i not found (is server running?)\n", input.serverHostname, input.serverPort);
        return -1;
    }

    if (!clientSocket.identifyUsername(input.username)) {
        printf("Could not send your username\n");
    }

    clientUsername = input.username;
    serverDescriptor = clientSocket.getSocketDescriptor();

    fileHandler.createDir();

    pthread_t connectionThread, notifyThread;
    pthread_create(&connectionThread, NULL, handleServerAnswers, NULL);
    pthread_create(&notifyThread, NULL, handleNotifyEvents, NULL);

    clientSocket.getSyncDir();

    inputHandler.setClientSyncDirName((char*) fileHandler.getDirname().c_str());

    bool shouldExit = false;
    while (!shouldExit) {
        char inputCommand[INPUT_SIZE] = "";
        fgets(inputCommand, INPUT_SIZE, stdin);
        Input input = inputHandler.processCommand(inputCommand);
        switch (input.inputCode) {
            case INPUT_EXIT:
                clientSocket.disconnectFromServer();
                shouldExit = true;
                break;
            case INPUT_UPLOAD: {
                WrappedFile file = fileHandler.getFile(input.args.fileToUpload);
                string filepath = fileHandler.getFilepath(file.filename);
                notifier.stopWatching();
                fileHandler.createFile(filepath.c_str(), file.content.c_str(), file.filesize);
                notifier.startWatching();
                if(!clientSocket.uploadFileToServer(file))
                    printf("Could not upload your file\n");
                break;
            }
            case INPUT_DOWNLOAD:
                if (!clientSocket.askToDownloadFile(input.args.fileToDownload))
                    printf("Could not download your file\n");
                break;
            case INPUT_DELETE: {
                string filepath = fileHandler.getFilepath(input.args.fileToDelete);
                notifier.stopWatching();
                fileHandler.deleteFile(filepath.c_str());
                notifier.startWatching();
                if(!clientSocket.deleteFile(input.args.fileToDelete))
                    printf("Could not delete your file\n");
                break;
            }
            case INPUT_LIST_CLIENT: {
                vector<FileForListing> files = fileHandler.getFiles();
                fileHandler.printFileList(files);
                break;
            }
            case INPUT_LIST_SERVER:
                clientSocket.askForFileList();
                break;
            case INPUT_GET_SYNC_DIR:
                fileHandler.createDir();
                break;
        }
    }

    clientSocket.closeSocket();
    return 0;
}
