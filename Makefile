CC = g++ -std=c++11

SRC = src
APP = application

all: wrappers handlers
	${CC} -g -o server ${APP}/ServerApplication.cpp ServerSocketWrapper.o SocketWrapper.o PacketHandler.o ConnectionHandler.o FileHandler.o -pthread
	${CC} -g -o client ${APP}/ClientApplication.cpp ClientSocketWrapper.o SocketWrapper.o PacketHandler.o FileHandler.o -pthread

wrappers:
	${CC} -g -c ${SRC}/SocketWrapper.cpp
	${CC} -g -c ${SRC}/ClientSocketWrapper.cpp 
	${CC} -g -c ${SRC}/ServerSocketWrapper.cpp 

handlers:
	${CC} -g -c ${SRC}/PacketHandler.cpp
	${CC} -g -c ${SRC}/ConnectionHandler.cpp
	${CC} -g -c ${SRC}/FileHandler.cpp

clean:
	rm *.o
	rm client
	rm server