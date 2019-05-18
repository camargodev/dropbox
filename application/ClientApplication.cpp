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

vector<FileForListing> receivedFileList;
SocketDescriptor serverDescriptor;
ClientSocketWrapper clientSocket;
PacketHandler packetHandler;
FileHandler fileHandler;
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
            input.args.directory = fileHandler.getDirname();
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

void handleDownloadedFile(Packet* packet) {
	// packetHandler.addPacketToReceivedFile(socket, packet->filename, packet);
	string filenameToSave = getCorrectFilename(packet->filename);
	File* file = fopen(filenameToSave.c_str(), "a"); 
	fwrite(&(packet->payload), 1, packet->payloadSize, file);
	fclose(file);
}

void handleReceivedPacket(Packet* packet) {
    switch (packet->command) {
        case SYNC_FILE:
            printf("I must download file %s\n", packet->filename);
            if (!clientSocket.askToDownloadFile(packet->filename))
                printf("Could not download your file\n");
            break;
        case DOWNLOADED_FILE:
            handleDownloadedFile(packet);
            // packetHandler.addPacketToReceivedFile(serverDescriptor, packet->filename, packet);
            // if (packet->currentPartIndex == packet->numberOfParts) {
            //     string content = packetHandler.getFileContent(serverDescriptor, packet->filename);
            //     int contentSize = packetHandler.getFileContentSize(serverDescriptor, packet->filename);
            //     printf("\nI downloaded file %s with payload:\n", packet->filename);
            //     // fileHandler.createFile(packet->filename, content, contentSize);
			// 	packetHandler.removeFileFromBeingReceivedList(serverDescriptor, packet->filename);
            // }
			break;
        case SIMPLE_MESSAGE:
            printf("I received a simple message from server: %s\n", packet->payload);
            break;
        case ERROR_MESSAGE:
            printf("Error with file %s: %s\n", packet->filename, packet->payload);
            break;
        case DELETE_ORDER:
            fileHandler.deleteFile(packet->filename);
            break;
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
            break;
    }
}

void *handleServerAnswers(void* dummy) {
    while (true) {
        Packet* packet = clientSocket.receivePacketFromServer();
        handleReceivedPacket(packet);
    }
}
using namespace std;

#define EVENT_SIZE  (sizeof (struct inotify_event))
#define BUF_LEN (EVENT_SIZE + 4096)
int notify_count = 0;
char* username;

void dealWithEvent(struct inotify_event *event, char* path){

    string name = event->name;
    if ((event->len == 32) || (name.find(".") == 0)){
        return;
    }
    cout << "========="<<notify_count<<"==========="<<endl;
    notify_count++;
    
    char path_file[500] = {0};
    strcat(path_file, path);
    strcat(path_file, "/");
    strcat(path_file, event->name);
    cout << path_file<< endl;

    if(event->mask & IN_CLOSE_WRITE || event->mask & IN_MOVED_TO){
        cout << "CRIOU/EDITOU um arquivo" << endl;
        WrappedFile file = fileHandler.getFile(path_file);
        if (!clientSocket.uploadFileToServer(file))
            printf("Could not send your file\n");

    }
    if(event->mask & IN_MOVED_FROM){
        cout << "DELETOU um arquivo" <<endl;
        if (!clientSocket.deleteFile(event->name))
            printf("Could not delete your file\n");
    }

    cout << "Name: " << event->name << endl;
}

void checkForUpdates() {
    char* path_name = fileHandler.getDirpath();
    cout << path_name <<endl;
    int fd = inotify_init1(IN_NONBLOCK);
    int wd = inotify_add_watch(fd, path_name, IN_CLOSE_WRITE | IN_MOVED_FROM | IN_MOVED_TO);
    char *buffer[BUF_LEN];

    int size_read, i;
    while(true) {
        i = 0;
        size_read = read(fd, buffer, BUF_LEN);
        if(size_read > 0) {

            inotify_event* event = (struct inotify_event*) buffer;
            dealWithEvent(event, path_name);
            i += event->len + EVENT_SIZE;

        }
    }

//    cout << "Watcher: " << watcher << endl;
}

void *handleNotifyEvents(void* dummy) {
 
    checkForUpdates();
    
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
    username = input.username;
    serverDescriptor = clientSocket.getSocketDescriptor();

    fileHandler.createDir();
    clientSocket.getSyncDir();

    pthread_t connectionThread, notifyThread;
    printf("Creating thread to get server answers...\n");
    pthread_create(&connectionThread, NULL, handleServerAnswers, NULL);
    pthread_create(&notifyThread, NULL, handleNotifyEvents, NULL);

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
                WrappedFile file = fileHandler.getFile(input.args.fileToUpload);
                if (!clientSocket.uploadFileToServer(file))
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
                fileHandler.printFileList(fileHandler.getFiles());
                break;
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
