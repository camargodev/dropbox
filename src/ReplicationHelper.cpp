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

void ReplicationHelper :: addMirror(Mirror mirror) {
    sem_wait(&processing);
    this->mirrors.push_back(mirror);
    sem_post(&processing);
}

vector<Mirror>  ReplicationHelper :: getMirrors() {
    return this->mirrors;
}