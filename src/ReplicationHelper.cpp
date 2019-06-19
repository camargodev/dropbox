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

void ReplicationHelper :: removeMirrorFromList(Mirror mirrorToRemove) {
    sem_wait(&processing);
    int index = 0;
    for (auto mirror : this->mirrors) {
        if (strcmp(mirror.ip, mirrorToRemove.ip) == 0 && mirror.port == mirrorToRemove.port) {
            mirrors.erase(mirrors.begin() + index);
            break;
        }
        index += 1;
    }
    sem_post(&processing);
}

void ReplicationHelper :: addSocketToMirror(Mirror mirrorToAddSocket, SocketDescriptor socket) {
    sem_wait(&processing);
    for (auto&& mirror : this->mirrors) {
        if (strcmp(mirror.ip, mirrorToAddSocket.ip) == 0 && mirror.port == mirrorToAddSocket.port) {
            mirror.socket = socket;
            printf("Setting socket %i to mirror %s:%i\n", socket, mirrorToAddSocket.ip, mirrorToAddSocket.port);
            break;
        }
    }
    sem_post(&processing);
}