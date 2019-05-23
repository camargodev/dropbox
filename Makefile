CC = g++ -std=c++11

OBJ = objects
SRC = src
APP = application
DOWN = downloads

all: wrappers handlers notifier cleaner
	${CC} -g -o server ${APP}/ServerApplication.cpp ${OBJ}/ServerSocketWrapper.o ${OBJ}/SocketWrapper.o ${OBJ}/PacketHandler.o ${OBJ}/ConnectionHandler.o ${OBJ}/ServerFileHandler.o ${OBJ}/ClientFileHandler.o -pthread
	${CC} -g -o client ${APP}/ClientApplication.cpp ${OBJ}/ClientSocketWrapper.o ${OBJ}/SocketWrapper.o ${OBJ}/PacketHandler.o ${OBJ}/ClientFileHandler.o ${OBJ}/Notifier.o -pthread

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

notifier:
	${CC} -g -c ${SRC}/Notifier.cpp
	
cleaner:
	mkdir -p ${OBJ}
	mkdir -p ${DOWN}
	mv *.o ${OBJ} 

clean:
	rm -f ${OBJ}/*.o
	rm -f client
	rm -f server