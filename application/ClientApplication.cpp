#include <sys/inotify.h>
#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string>

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
char* clientUsername;

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
    cout << filename <<endl;
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
            input.args.directory = (char *)fileHandler.getDirname().c_str();
            break;
    }
    return input;
}

string getCorrectFilename(const string& s) { 
	char* home = getenv("HOME");    
   char sep = '/';
   size_t i = s.rfind(sep, s.length());
   if (i != string::npos) {
      string f = s.substr(i+1, s.length() - i);
	  char filename[300];  
	  ::sprintf(filename, "%s/sync_dir/%s", home, f.c_str()); 
	 return filename;
   }
   return("");
}

void handleReceivedPacket(Packet* packet) {
    switch (packet->command) {
        case SYNC_FILE:
            printf("I must download file %s\n", packet->filename);
            if (!clientSocket.askToDownloadFile(packet->filename))
                printf("Could not download your file\n");
            break;
        case DOWNLOADED_FILE: {
            string filepath = fileHandler.getFilepath(packet->filename);
            if(packet->currentPartIndex == 1)
                fileHandler.createFile(filepath.c_str(), packet->payload, packet->payloadSize);
            else
                fileHandler.appendFile(filepath.c_str(), packet->payload, packet->payloadSize);
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

void *handleNotifyEvents(void* dummy) {
    Notifier notifier(fileHandler.getDirpath());
    while(true) {
        Action action = notifier.getListenedAction();
        if (action.type == Notifier::NO_ACTION)
            continue;
        char actionFilename[FILENAME_SIZE];
        strcpy(actionFilename, action.filename.c_str());
        switch(action.type) {
            case Notifier::CREATE:
                handleFileUpdate(actionFilename);
                break;
            case Notifier::EDIT:
                handleFileUpdate(actionFilename);
                break;
            case Notifier::DELETE:
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
            case INPUT_UPLOAD: {
                WrappedFile file = fileHandler.getFile(input.args.fileToUpload);
                if(!clientSocket.uploadFileToServer(file))
                    printf("Could not upload your file\n");
                break;
            }
            case INPUT_DOWNLOAD:
                if (!clientSocket.askToDownloadFile(input.args.fileToDownload))
                    printf("Could not download your file\n");
                break;
            case INPUT_DELETE: {
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
