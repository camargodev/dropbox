all:
	g++ -c server.cpp SocketWrapper.cpp
	g++ -c client.cpp
	g++ -o server server.o SocketWrapper.o 
	g++ -o client client.o