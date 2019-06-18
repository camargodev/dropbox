#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "../include/ServerSocketWrapper.hpp"
#include "../include/ClientSocketWrapper.hpp"
#include "../include/ConnectionHandler.hpp"
#include "../include/ServerFileHandler.hpp"
#include "../include/ReplicationHelper.hpp"
#include "../include/ElectionHelper.hpp"
#include "../include/AddressGetter.hpp"

void startElection();

ConnectionHandler connHandler;
ServerSocketWrapper serverSocket;
ClientSocketWrapper clientSocket;
ServerFileHandler fileHandler;
ReplicationHelper replicationHelper;
ElectionHelper electionHelper;
sem_t handling;
int myPort;

int getServerPort(int argc, char *argv[]) {
	int port = stoi(string(argv[1]));
    printf("I will answer on port %i\n", port);
	return port;
}

bool receivedFromTheCurrentOpenSocket(SocketDescriptor originSocket, SocketDescriptor openSocket) {
	return originSocket == openSocket;
}

bool messageRequiresUserAlreadyConnected(Command command) {
    return !(command == IM_ALIVE || command == MIRROR 
        || command == MIRROR_REPLICATION || command == SIMPLE_MESSAGE
        || command == IDENTIFICATION || command == ELECTION
        || command == ANSWER || command == COORDINATOR);
}

bool handleReceivedPacket(int socket, Packet* packet) {
    bool shouldKeepExecuting = true;
	string filenameToSave;
    ConnectedUser connectedClient = connHandler.getConnectedClientBySocket(socket);
    
    if (!connectedClient.valid && messageRequiresUserAlreadyConnected(packet->command)) {
        printf("I need a user and could not find a valid one\n");
        return shouldKeepExecuting;
    }
	
    switch (packet->command) {
        case MIRROR: {
            printf("I will send everything to my mirror on socket %i in %s:%i\n", socket, packet->ip, packet->port);
            Mirror mirror = Mirror(socket, packet->ip, packet->port);
            
            // send all existing mirror to the new; send the new to all existing
            for (auto existingMirror : replicationHelper.getMirrors()) {
                serverSocket.sendMirror(existingMirror.socket, mirror);
                serverSocket.sendMirror(mirror.socket, existingMirror);
            }
            replicationHelper.addMirror(mirror);
            break;
        }

        case MIRROR_REPLICATION: {
            Mirror mirror = Mirror(socket, packet->ip, packet->port);
            replicationHelper.addMirror(mirror);
            printf("\nMirrors I know:\n");
            for (auto existingMirror : replicationHelper.getMirrors()) {
                printf("  Mirror in %s:%i\n", existingMirror.ip, existingMirror.port);
            }
            break;
        }

        case SIMPLE_MESSAGE:
            printf("I received a simple message: %s\n", packet->payload);
            break;

		case GET_SYNC_DIR: {
            printf("Client %s requested sync dir\n", connectedClient.username.c_str());
            vector <FileForListing> filesOnUserDirectory = fileHandler.getFiles(connectedClient.username.c_str());

            printf("\nI will send %s's file list on socket %i\n", connectedClient.username.c_str(), socket);
            serverSocket.sendFileListForSyncDir(socket, filesOnUserDirectory);

            break;
        }

        case UPLOAD_FILE: {
            printf("I received file %s (%i/%i)\n", packet->filename, packet->currentPartIndex, packet->numberOfParts);
            if(packet->currentPartIndex == 1)
                fileHandler.createFile(connectedClient.username.c_str(), packet->filename, packet->payload, packet->payloadSize);
            else
                fileHandler.appendFile(connectedClient.username.c_str(), packet->filename, packet->payload, packet->payloadSize);

            if (replicationHelper.isMainServer()) {
                if (packet->currentPartIndex == packet->numberOfParts) {
                    WrappedFile file = fileHandler.getFile(connectedClient.username.c_str(), packet->filename);
                    for (auto openConnection : connectedClient.openConnections) {
                        if (!receivedFromTheCurrentOpenSocket(socket, openConnection.socket)) {
                            printf("GOT FROM: %i! I will send to client %s on socket %i\n", socket, openConnection.ip, openConnection.socket);
                            serverSocket.sendSyncFile(openConnection.socket, file);
                        }
                    }
                }
                for (auto mirror : replicationHelper.getMirrors()) {
                    printf ("I will replicate to mirror %s:%i\n", mirror.ip, mirror.port);
                    serverSocket.sendPacketToClient(mirror.socket, packet);
                }
            }
            break;
        }

		case DOWNLOAD_REQUISITION: {
            // printf("\nI'll try to send file %s to client of socket %i\n", packet->payload, socket);

            WrappedFile file = fileHandler.getFile(connectedClient.username.c_str(), packet->payload);
            serverSocket.sendDownloadedFile(socket, file);

            break;
        }

        case ASK_FOR_SYNC_FILE: {
            // printf("\nI'll try to send file %s to client of socket %i\n", packet->payload, socket);

            WrappedFile file = fileHandler.getFile(connectedClient.username.c_str(), packet->payload);
            serverSocket.sendSyncFile(socket, file);

            break;
        }

		case DELETE_REQUISITION: {
            fileHandler.deleteFile(connectedClient.username.c_str(), packet->filename);

            if (replicationHelper.isMainServer()) {
                Packet answer(DELETE_ORDER, packet->filename);
                for (auto openConnection : connectedClient.openConnections) {
                    if (!receivedFromTheCurrentOpenSocket(socket, openConnection.socket))
                        serverSocket.sendPacketToClient(openConnection.socket, &answer);
                }
                for (auto mirror : replicationHelper.getMirrors()) 
                    serverSocket.sendPacketToClient(mirror.socket, packet);
            }
            break;
        }

		case IDENTIFICATION: {
            printf("\nClient %s connected on socket %i with IP %s\n", packet->payload, socket, packet->ip);

            fileHandler.createClientDir(packet->payload);
            connHandler.addSocketToClient(packet->payload, ClientInfo(socket, packet->ip));

            if (replicationHelper.isMainServer()) {
                // printf("I will send this identification to all my mirrors\n");
                for (auto mirror : replicationHelper.getMirrors()) {
                    if (serverSocket.sendPacketToClient(mirror.socket, packet))
                        printf("Sent %s identification to mirror %s:%i\n", packet->payload, mirror.ip, mirror.port);                    
                }
            }
            break;
        }

		case DISCONNECT: {
            printf("\nClient %s disconnected on socket %i\n", connectedClient.username.c_str(), socket);

            connHandler.removeSocketFromUser(connectedClient.username, socket);
            shouldKeepExecuting = false;

            if (replicationHelper.isMainServer()) 
                for (auto mirror : replicationHelper.getMirrors())
                    serverSocket.sendPacketToClient(mirror.socket, packet);

            break;
        }

		case LIST_REQUISITION: {
            vector <FileForListing> filesOnUserDir = fileHandler.getFiles(connectedClient.username.c_str());

            printf("\nI will send %s's file list on socket %i\n", connectedClient.username.c_str(), socket);
            serverSocket.sendFileList(socket, filesOnUserDir);

            break;
        }

        case IM_ALIVE: {
            replicationHelper.lastSignalFromServer = clock();
            break;
        }

        case ELECTION: {
            ClientSocketWrapper miniClientSocket;
            printf("Mirror %s:%i sent a ELECTION\n", packet->ip, packet->port);
            startElection();
            if (!miniClientSocket.setServer(packet->ip, packet->port))
                printf("Error setting server\n");
            if (!miniClientSocket.connectToServer())
                printf("Error connecting\n");
            miniClientSocket.sendElectionAnswer();
            break;
        }

        case ANSWER: {
            if (!electionHelper.hasReceivedAnswer())
                printf("I received an answer and will wait for coord\n");
            electionHelper.confirmAnswerReceived();
        }
    }
	return shouldKeepExecuting;
}

void *handleNewConnection(void *voidSocket) {
	int socket = *(int*) voidSocket;
    // printf("Handling connection on socket %i\n", socket);
	bool shouldKeepExecuting = true;
    while(shouldKeepExecuting) {
		Packet* packet = serverSocket.receivePacketFromClient(socket);
        if (packet != NULL)
		    shouldKeepExecuting = handleReceivedPacket(socket, packet);
	}
    return nullptr;
}

void *handleMainServerAnswers(void *voidSocket) {
    int socket = *(int*) voidSocket;
    while(!replicationHelper.isMainServer()) {
        Packet* packet = clientSocket.receivePacketFromServer(5);
        if (packet != NULL)
            handleReceivedPacket(socket, packet);
	}
    return nullptr;
}

void identifyAsCoordinator() {
    printf("I'm the NEW SERVER\n");
    ClientSocketWrapper miniClientSocket;
    for (auto user : connHandler.getAllConnectedUsers()) {
        for (auto connection : user.openConnections) {
            printf("Warning user %s: client %s:%i\n", user.username.c_str(), connection.ip, ReplicationHelper::PORT_TO_NEW_SERVER);
            if (!miniClientSocket.setServer(connection.ip, ReplicationHelper::PORT_TO_NEW_SERVER))
                printf("Error setting server\n");
            if (!miniClientSocket.connectToServer())
                printf("Error connecting\n");
            miniClientSocket.identifyAsNewServer(serverSocket.getPort());
            // miniClientSocket.closeSocket();
        }
    }
    printf("Now everybody knows I'm the main server\n");
    replicationHelper.setAsMainServer();
}

void *checkForElectionAnswersOnNewThread(void *dummy) {
    bool isTheNewCoordinator = false;
    while (!electionHelper.hasReceivedAnswer() && !isTheNewCoordinator) {
        Clock clocksWithoutAnswer = clock() - electionHelper.getClockWhenElectionStarted();
        double secondsWithoutAnswer = ((double) clocksWithoutAnswer)/CLOCKS_PER_SEC;
        isTheNewCoordinator = secondsWithoutAnswer >= ElectionHelper::TIMEOUT_FOR_COORDINATOR;
    }
    if (isTheNewCoordinator)
        printf("I AM THE NEW COORDINATOR\n");
    return nullptr;
}

void checkForElectionAnswers() {
    pthread_t electionThread;
    pthread_create(&electionThread, NULL, checkForElectionAnswersOnNewThread, NULL);
}

void startElection() {
    AddressGetter addressGetter;
    if (electionHelper.hasAlreadyStartedElection())
        return;
    checkForElectionAnswers();
    Mirror me = Mirror(addressGetter.getIP(), myPort);
    auto mirrorsWithHigherPrio = electionHelper.getMirrorsWithHighestPrio(me, replicationHelper.getMirrors());
    for (auto mirror : mirrorsWithHigherPrio) {
        ClientSocketWrapper miniClientSocket;
        if (!miniClientSocket.setServer(mirror.ip, mirror.port))
            printf("Error setting server for miniClient\n");
        if (!miniClientSocket.connectToServer())
            printf("Error connecting\n");
        if (!miniClientSocket.sendElectionMessage(me))
            printf("Error sending election\n");
    }
    electionHelper.setElectionAsStarted();
}

void *processLivenessOnNewThread(void *dummy) {
    while(true) {
        if (replicationHelper.isMainServer()){
            for (auto mirror : replicationHelper.getMirrors()) {
                // printf("Sending Im alive\n");
                Packet packet(IM_ALIVE);
                serverSocket.sendPacketToClient(mirror.socket, &packet);
            }
        } else {
            Clock clocksWithoutSignal = clock() - replicationHelper.lastSignalFromServer;
            double timeWithoutSignal = ((double) clocksWithoutSignal)/CLOCKS_PER_SEC;
            if (timeWithoutSignal >= ReplicationHelper::TIMEOUT_TO_START_ELECTION) {
                startElection();
                break;
            }
        }
        sleep(ReplicationHelper::LIVENESS_NOTIFICATION_DELAY);
	}
    return nullptr;
}

bool isMirror(int argc) {
    return argc > 3;
}

void processNewClientConnected() {
    pthread_t connectionThread;
    // printf("Waiting for a new client to connect\n");
    Connection clientConnection = serverSocket.acceptClientConnection();
    int descriptor = clientConnection.descriptor;
    if (descriptor < 0) 
        return;
    pthread_create(&connectionThread, NULL, handleNewConnection, &descriptor);
}

void processMainServerAnswers() {
    pthread_t connectionThread;
    int descriptor = clientSocket.getSocketDescriptor();
    pthread_create(&connectionThread, NULL, handleMainServerAnswers, &descriptor);
    // pthread_join(connectionThread, NULL);
}

void processLiveness() {
    pthread_t connectionThread;
    pthread_create(&connectionThread, NULL, processLivenessOnNewThread, NULL);
}

void connectAsMirror(char *argv[]) {
    if (!clientSocket.setServer(string(argv[2]), stoi(string(argv[3])))) {
        printf("Error setting server\n");
        return;
    }
    if (!clientSocket.connectToServer()) {
        printf("Error connecting to main server\n"); 
        return;
    }
    if (!clientSocket.identifyAsMirror(stoi(string(argv[1])))) {
        printf("Error sending identification\n");
        return;
    }
    printf("I will be a mirror of %s:%s\n", argv[2], argv[3]);
}

int main(int argc, char *argv[]) {

    myPort = getServerPort(argc, argv);
	serverSocket.listenOnPort(myPort);
    sem_init(&handling, 0, 1);
    
    if (isMirror(argc)) {
        connectAsMirror(argv);
        replicationHelper.setAsBackupServer();
        fileHandler.configAsBackup();
    }

	if (!serverSocket.openSocket()) {
		printf("Could not open socket. Try another port\n");
		return -1;
	}

	serverSocket.setNumberOfClients(5);
	fileHandler.createDir();
    
    processLiveness();

    if (!replicationHelper.isMainServer()) { 
        processMainServerAnswers();   
    } 
    
    while (true) {  
        processNewClientConnected(); 
    }

	serverSocket.closeSocket();
	return 0;
}
