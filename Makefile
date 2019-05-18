CC = g++ -std=c++11

OBJ = objects
SRC = src
APP = application

all: wrappers handlers
	${CC} -g -o server ${APP}/ServerApplication.cpp ${OBJ}/ServerSocketWrapper.o ${OBJ}/SocketWrapper.o ${OBJ}/PacketHandler.o ${OBJ}/ConnectionHandler.o ${OBJ}/ServerFileHandler.o ${OBJ}/ClientFileHandler.o -pthread
	${CC} -g -o client ${APP}/ClientApplication.cpp ${OBJ}/ClientSocketWrapper.o ${OBJ}/SocketWrapper.o ${OBJ}/PacketHandler.o ${OBJ}/ClientFileHandler.o -pthread

wrappers:
	${CC} -g -c ${SRC}/SocketWrapper.cpp
	${CC} -g -c ${SRC}/ClientSocketWrapper.cpp 
	${CC} -g -c ${SRC}/ServerSocketWrapper.cpp 
	${CC} -g -c ${SRC}/ServerSocketWrapper.cpp 

handlers:
	${CC} -g -c ${SRC}/PacketHandler.cpp
	${CC} -g -c ${SRC}/ConnectionHandler.cpp
	${CC} -g -c ${SRC}/ClientFileHandler.cpp
	${CC} -g -c ${SRC}/ServerFileHandler.cpp
	mv *.o objects 

clean:
	rm ${OBJ}/*.o
	rm client
	rm server
	rm test
