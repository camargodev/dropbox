CC = g++

SRC = src

all: wrappers
	${CC} -o server ServerApplication.cpp ServerSocketWrapper.o SocketWrapper.o
	${CC} -o client ClientApplication.cpp ClientSocketWrapper.o SocketWrapper.o

wrappers:
	${CC} -c ${SRC}/SocketWrapper.cpp
	${CC} -c ${SRC}/ClientSocketWrapper.cpp 
	${CC} -c ${SRC}/ServerSocketWrapper.cpp 

clean:
	rm *.o
	rm client
	rm server