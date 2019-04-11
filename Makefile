all:
	g++ -c SocketWrapper.cpp
	g++ -c ClientSocketWrapper.cpp 
	g++ -c ServerSocketWrapper.cpp 
	g++ -o server server.cpp ServerSocketWrapper.o SocketWrapper.o
	g++ -o client client.cpp ClientSocketWrapper.o SocketWrapper.o