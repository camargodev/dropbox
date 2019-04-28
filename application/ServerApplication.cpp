#include <stdio.h>
#include "../include/ServerSocketWrapper.hpp"
#include "../include/PacketHandler.hpp"

int getServerPort(int argc, char *argv[]) {
	int port = SocketWrapper::DEFAULT_PORT;
	if (argc == 2)
        port = stoi(string(argv[1]));
	return port;
}

int main(int argc, char *argv[]) {
	ServerSocketWrapper serverSocket(getServerPort(argc, argv));
	PacketHandler packetHandler;

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
		packetHandler.handleReceivedPacket(clientConnection.descriptor, packet);

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