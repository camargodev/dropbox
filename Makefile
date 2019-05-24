compile = g++ -std=c++11 -g -Wno-pointer-arith

o = objects
src = src
app = application

client-dependencies = ${o}/client/
server-dependencies = ${o}/server/

with-client-dependencies = ${o}/client/*.o
with-server-dependencies = ${o}/server/*.o

multi-thread = -pthread

move-all-to = mv *.o
as = -o
file = -c

all: dir-gen client server
	${compile} ${as} server ${app}/ServerApplication.cpp ${with-server-dependencies} ${multi-thread}
	${compile} ${as} client ${app}/ClientApplication.cpp ${with-client-dependencies} ${multi-thread}

client:
	${compile} ${file} ${src}/Notifier.cpp
	${compile} ${file} ${src}/InputHandler.cpp
	${compile} ${file} ${src}/ClientFileHandler.cpp
	${compile} ${file} ${src}/SocketWrapper.cpp
	${compile} ${file} ${src}/ClientSocketWrapper.cpp 
	${move-all-to} ${client-dependencies}

server:
	${compile} ${file} ${src}/PacketHandler.cpp
	${compile} ${file} ${src}/SocketWrapper.cpp
	${compile} ${file} ${src}/ServerSocketWrapper.cpp 
	${compile} ${file} ${src}/ConnectionHandler.cpp
	${compile} ${file} ${src}/ClientFileHandler.cpp
	${compile} ${file} ${src}/ServerFileHandler.cpp
	${move-all-to} ${server-dependencies}

dir-gen:
	mkdir -p ${client-dependencies}
	mkdir -p ${server-dependencies}
	mkdir -p downloads

clean:
	rm -f ${client-dependencies}/*.o
	rm -f ${client-dependencies}/*.o
	rm -f client
	rm -f server