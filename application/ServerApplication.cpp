#include <stdio.h>
#include <iostream>
#include "../include/ServerSocketWrapper.hpp"
#include "../include/PacketHandler.hpp"
#include "../include/ConnectionHandler.hpp"

PacketHandler packetHandler;
ConnectionHandler connHandler;

int getServerPort(int argc, char *argv[]) {
	int port = SocketWrapper::DEFAULT_PORT;
	if (argc == 2)
        port = stoi(string(argv[1]));
	return port;
}

void handleReceivedPacket(int socket, Packet* packet) {
    switch (packet->command) {
        case UPLOAD_FILE:
            packetHandler.addPacketToReceivedFile(socket, packet->filename, packet);
            if (packet->currentPartIndex == packet->numberOfParts) {
                string content = packetHandler.getFileContent(socket, packet->filename);
                printf("I received file %s with payload:\n%s\n", packet->filename, content.c_str());
				ConnectedClient client = connHandler.getConnectedClientBySocket(socket);
				printf("Now I should notify user %s with his sockets", client.username.c_str());
                packetHandler.removeFileFromBeingReceivedList(socket, packet->filename);
            }
            break;
        
		case IDENTIFICATION:
            printf("Client %s connected on socket %i\n", packet->payload, socket);
			connHandler.addSocketToClient(packet->payload, socket);
            break;

		case DISCONNECT:
			ConnectedClient client = connHandler.getConnectedClientBySocket(socket);
			printf("Client %s disconnected on socket %i", client.username.c_str(), socket); 
			connHandler.removeSocketFromUser(client.username, socket);
    }
}

int main(int argc, char *argv[]) {
	ServerSocketWrapper serverSocket(getServerPort(argc, argv));

	if (!serverSocket.openSocket()) {
		printf("Could not open socket\n");
		return -1;
	}
	serverSocket.setNumberOfClients(5);

	Connection clientConnection = serverSocket.acceptClientConnection();
	// Um pacote inicial é recebido com o username
	// Packet* packet = serverSocket.receivePacketFromClient(clientConnection.descriptor);
	// printf("\nClient %s conectado\n", packet->payload);
	
	while (true) {

		// printf("\nWaiting to receive packet");
		Packet* packet = serverSocket.receivePacketFromClient(clientConnection.descriptor);
		handleReceivedPacket(clientConnection.descriptor, packet);

		// bool receivedFullFile = false;
		// string fullPayload = "";

		// // Código para receber pacotes até finalizar um arquivo
		// while (!receivedFullFile) {
		// 	packet = serverSocket.receivePacketFromClient(clientConnection.descriptor);	
		// 	fullPayload += string(packet->payload);
		// 	receivedFullFile = (packet->currentPartIndex == packet->numberOfParts);
		// }

		// // Quando chega aqui, já tem o conteúdo completo do arquivo em fullPayload
		// // Todas as outras informações são iguais em todos os pacotes de um arquivo,
		// //		portanto o último pacote respondido já vai ter as infos
		// switch (packet->command) {
		// 	case UPLOAD_FILE:
		// 		printf("\nRECEIVED %s:\n%s\n", packet->filename, fullPayload.c_str());
		// }
		
	}

	serverSocket.closeSocket();
	return 0; 
}