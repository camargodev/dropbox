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
AddressGetter addressGetter;
sem_t waitingForElectionResults;
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
        || command == ANSWER || command == COORDINATOR
        || command == MIRROR_UPDATE);
}

bool handleReceivedPacket(int socket, Packet* packet) {
    bool shouldKeepExecuting = true;
	string filenameToSave;
    ConnectedUser connectedClient = connHandler.getConnectedClientBySocket(socket);
    
    if (!connectedClient.valid && messageRequiresUserAlreadyConnected(packet->command)) {
        printf("I need a user for command %i and could not find a valid one\n", packet->command);
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
            Mirror mirror = Mirror(-1, packet->ip, packet->port);
            replicationHelper.addMirror(mirror);
            printf("I know mirror %s:%i\n", packet->ip, packet->port);
            break;
        }

        case MIRROR_UPDATE: {
            Mirror mirror = Mirror(packet->ip, packet->port);
            replicationHelper.addSocketToMirror(mirror, socket);
            Packet answer(SIMPLE_MESSAGE);
            serverSocket.sendPacketToClient(socket, &answer);
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
            // printf("My server is alive\n");
            break;
        }

        case ELECTION: {
            ClientSocketWrapper miniClientSocket;
            // printf("ELECTION FROM %s:%i\n", packet->ip, packet->port);
            // sem_wait(&waitingForElectionResults);
            if (!electionHelper.hasAlreadyStartedElection()) {
                // printf("ON ELECTION RECEIVED!\n");
                startElection();
            }
            // sem_post(&waitingForElectionResults);
            if (!miniClientSocket.setServer(packet->ip, packet->port))
                printf("Error setting server\n");
            if (!miniClientSocket.connectToServer())
                printf("Error connecting\n");
            miniClientSocket.sendElectionAnswer(Mirror(addressGetter.getIP(), myPort));
                // printf("ANSWER TO %s:%i\n", packet->ip, packet->port);
            break;
        }

        case ANSWER: {
            // printf("ANSWER FROM: %s:%i\n", packet->ip, packet->port);
            electionHelper.confirmAnswerReceived();
            break;
        }

        case COORDINATOR: {
            printf("NEW COORDINATOR: %s:%i\n", packet->ip, packet->port);
            clientSocket.closeSocket();

            if (!clientSocket.setServer(packet->ip, packet->port))
                printf("Error setting server\n");
            if (!clientSocket.connectToServer())
                printf("Error connecting\n");
            // clientSocket.sendMirrorForUpdate(myPort);

            replicationHelper.removeMirrorFromList(Mirror(packet->ip, packet->port));
            replicationHelper.lastSignalFromServer = clock();
            electionHelper.reset();
            
            break;
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

void *handleMainServerAnswers(void *dummy) {
    while(!replicationHelper.isMainServer()) {
        Packet* packet = clientSocket.receivePacketFromServer(3);
        if (packet != NULL) {
            handleReceivedPacket(clientSocket.getSocketDescriptor(), packet);
        }
	}
    return nullptr;
}

void identifyAsCoordinator() {
    printf("NEW COORDINATOR: me\n");
    ClientSocketWrapper miniClientSocket;
    for (auto user : connHandler.getAllConnectedUsers()) {
        for (auto connection : user.openConnections) {
            printf("Warning user %s: client %s:%i\n", user.username.c_str(), connection.ip, ReplicationHelper::PORT_TO_NEW_SERVER);
            if (!miniClientSocket.setServer(connection.ip, ReplicationHelper::PORT_TO_NEW_SERVER))
                printf("Error setting server\n");
            if (!miniClientSocket.connectToServer())
                printf("Error connecting\n");
            miniClientSocket.identifyAsNewServer(serverSocket.getPort());
            miniClientSocket.closeSocket();
        }
    }
    for (auto mirror : replicationHelper.getMirrors()) {
        if (mirror.socket == -1) {
            // printf("COORD TO %s:%i\n", mirror.ip, mirror.port);
            if (!miniClientSocket.setServer(mirror.ip, mirror.port))
                printf("Error setting server\n");
            if (!miniClientSocket.connectToServer())
                printf("Error connecting\n");
            replicationHelper.addSocketToMirror(mirror, miniClientSocket.getSocketDescriptor());
        }
        miniClientSocket.identifyAsNewCoordinator(serverSocket.getPort());
        // miniClientSocket.closeSocket();
    }
    // printf("Now everybody knows I'm the main server\n");
    replicationHelper.setAsMainServer();
}

void *checkForElectionAnswersOnNewThread(void *dummy) {
    bool isTheNewCoordinator = false;
    while (!electionHelper.hasReceivedAnswer() && !isTheNewCoordinator) {
        Clock clocksWithoutAnswer = clock() - electionHelper.getClockWhenElectionStarted();
        double secondsWithoutAnswer = ((double) clocksWithoutAnswer)/CLOCKS_PER_SEC;
        //printf("%fs without signal\n", secondsWithoutAnswer);
        isTheNewCoordinator = secondsWithoutAnswer >= ElectionHelper::TIMEOUT_FOR_ANSWER;
    }
    if (isTheNewCoordinator)
        identifyAsCoordinator();
    // printf("QUIT CHECK METHOD\n");
    return nullptr;
}

void checkForElectionAnswers() {
    pthread_t electionThread;
    pthread_create(&electionThread, NULL, checkForElectionAnswersOnNewThread, NULL);
}

void startElection() {
    //printf("I WILL START AN ELECTION\n");
    electionHelper.setElectionAsStarted();
    checkForElectionAnswers();
    Mirror me = Mirror(addressGetter.getIP(), myPort);
    auto mirrorsWithHigherPrio = electionHelper.getMirrorsWithHighestPrio(me, replicationHelper.getMirrors());
    for (auto mirror : mirrorsWithHigherPrio) {
        ClientSocketWrapper miniClientSocket;
        // if (!miniClientSocket.setServer(mirror.ip, mirror.port))
        //     printf("Error setting server for miniClient\n");
        // if (!miniClientSocket.connectToServer())
        //     printf("Error connecting\n");
        // if (!miniClientSocket.sendElectionMessage(me))
        //     printf("Error sending election\n");
        miniClientSocket.setServer(mirror.ip, mirror.port);
        miniClientSocket.connectToServer();
        miniClientSocket.sendElectionMessage(me);
        replicationHelper.addSocketToMirror(mirror, miniClientSocket.getSocketDescriptor());
        // printf("ELECTION TO: %s:%i\n", mirror.ip, mirror.port);
    }
}

void *processLivenessOnNewThread(void *dummy) {
    while(true) {
        if (replicationHelper.isMainServer()){
            for (auto mirror : replicationHelper.getMirrors()) {
                if (mirror.socket <= 0)
                    continue;
                // printf("Sending Im alive for %i (%s:%i)\n", mirror.socket, mirror.ip, mirror.port);
                Packet imAlivePacket(IM_ALIVE);
                serverSocket.sendPacketToClient(mirror.socket, &imAlivePacket);
            }
        } else if (!electionHelper.hasAlreadyStartedElection()) {
            Clock clocksWithoutSignal = clock() - replicationHelper.lastSignalFromServer;
            double timeWithoutSignal = ((double) clocksWithoutSignal)/CLOCKS_PER_SEC;
            // sem_wait(&waitingForElectionResults);
            if (timeWithoutSignal >= ReplicationHelper::TIMEOUT_TO_START_ELECTION) {
                    // printf("ON PROCESS LIVENESS WITH %f!\n", timeWithoutSignal);
                    startElection();
            }
            // sem_post(&waitingForElectionResults);
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
    pthread_create(&connectionThread, NULL, handleMainServerAnswers, NULL);
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
    sem_init(&waitingForElectionResults, 0, 1);
    
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
