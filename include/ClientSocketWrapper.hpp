#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "SocketWrapper.hpp"

using namespace std;

class ClientSocketWrapper : public SocketWrapper {

    public: 
        ClientSocketWrapper(string serverHostname, int serverPort);
        string readFromServer();
        void writeToServer(string message);

    private:
        sockaddr_in buildAddress(in_addr hostname, int port);

};

#endif