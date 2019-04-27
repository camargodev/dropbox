CC = g++

SRC = src
APP = application

all: wrappers
	${CC} -g -o server ${APP}/ServerApplication.cpp ServerSocketWrapper.o SocketWrapper.o
	${CC} -g -o client ${APP}/ClientApplication.cpp ClientSocketWrapper.o SocketWrapper.o

wrappers:
	${CC} -g -c ${SRC}/SocketWrapper.cpp
	${CC} -g -c ${SRC}/ClientSocketWrapper.cpp 
	${CC} -g -c ${SRC}/ServerSocketWrapper.cpp 

clean:
	rm *.o
	rm client
	rm server