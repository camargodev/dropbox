#include <stdio.h>
#include "../include/ServerSocketWrapper.hpp"

int getServerPort(int argc, char *argv[]) {
	int port = SocketWrapper::DEFAULT_PORT;
	if (argc == 2)
        port = stoi(string(argv[1]));
	return port;
}

int main(int argc, char *argv[])
{
	ServerSocketWrapper serverSocket(getServerPort(argc, argv));
	if (!serverSocket.openSocket()) {
		printf("\nCould not open socket");
		return -1;
	}
	serverSocket.setNumberOfClients(5);
	
	while (true) {

		printf("\nWaiting for client to connect");
		Connection clientConnection = serverSocket.acceptClientConnection();

		bool receivedFullFile = false;
		string fullPayload = "";

		// Recebe pacotes até completar o arquivo
		// No futuro, isso pode ser isolado em uma função
		Packet* packet;
		while (!receivedFullFile) {
			packet = serverSocket.receivePacketFromClient(clientConnection.descriptor);	
			fullPayload += string(packet->payload);
			receivedFullFile = (packet->currentPartIndex == packet->numberOfParts);
		}

		// Quando chega aqui, já tem o conteúdo completo do arquivo em fullPayload
		// Todas as outras informações são iguais em todos os pacotes de um arquivo,
		//		portanto o último pacote respondido já vai ter as infos
		switch (packet->command) {
			case UPLOAD_FILE:
				printf("\nNow I should save the file %s with payload: %s", packet->filename, fullPayload.c_str());
		}

		
		
		Packet answer; 
		strcpy(answer.payload, ("I received file " + string(packet->filename)).c_str());
		serverSocket.sendPacketToClient(clientConnection.descriptor, &answer);
		serverSocket.closeConnection(clientConnection);
	}

	serverSocket.closeSocket();
	return 0; 
}