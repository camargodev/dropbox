#ifndef REPLICATION_HELPER_HPP
#define REPLICATION_HELPER_HPP

#include "SocketWrapper.hpp"
#include "AddressGetter.hpp"
#include <semaphore.h>
#include <time.h> 

using Clock = clock_t;

struct Mirror {
    int socket;
    char ip[INET_ADDRSTRLEN];
    int port;

    Mirror(int socket, char ip[INET6_ADDRSTRLEN], int port) {
        this->socket = socket;
        strcpy(this->ip, ip);
        this->port = port;
    }

    Mirror(char ip[INET6_ADDRSTRLEN], int port) {
        this->socket = -1;
        strcpy(this->ip, ip);
        this->port = port;
    }
};

class ReplicationHelper {
public:
    const static int LIVENESS_NOTIFICATION_DELAY = 1;
    const static int TIMEOUT_TO_START_ELECTION   = 3;
    const static int PORT_TO_NEW_SERVER          = 4020;
    ReplicationHelper();
    bool isMainServer();
    void setAsMainServer();
    void setAsBackupServer();
    void addMirror(Mirror mirro);
    vector<Mirror> getMirrors();
    Clock lastSignalFromServer;
private:
    vector<Mirror> mirrors;
    bool isTheMainServer;
    sem_t processing;

};

#endif