#include "../include/ReplicationHelper.hpp"

ReplicationHelper :: ReplicationHelper() {
    this->setAsMainServer();
    sem_init(&processing, 0, 1);
}

bool ReplicationHelper :: isMainServer() {
    return this->isTheMainServer;
}

void ReplicationHelper :: setAsMainServer() {
    this->isTheMainServer = true;
}

void ReplicationHelper :: setAsBackupServer() {
    this->isTheMainServer = false;
}

void ReplicationHelper :: addMirror(SocketDescriptor mirrorSocket) {
    sem_wait(&processing);
    this->mirrors.push_back(mirrorSocket);
    sem_post(&processing);
}

vector<SocketDescriptor>  ReplicationHelper :: getMirrors() {
    return this->mirrors;
}