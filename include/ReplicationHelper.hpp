#ifndef REPLICATION_HELPER_HPP
#define REPLICATION_HELPER_HPP

#include "SocketWrapper.hpp"
#include <semaphore.h>

class ReplicationHelper {
public:
    ReplicationHelper();
    bool isMainServer();
    void setAsMainServer();
    void setAsBackupServer();
    void addMirror(SocketDescriptor mirrorSocket);
    vector<SocketDescriptor> getMirrors();
private:
    vector<SocketDescriptor> mirrors;
    bool isTheMainServer;
    sem_t processing;

};

#endif