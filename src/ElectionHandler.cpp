#include <vector>
#include <time.h>
#include <unistd.h>
#include "../include/Packet.hpp"
#include "../include/Command.hpp"
#include "../include/ElectionHandler.hpp"
#include "../include/ServerSocketWrapper.hpp"

using namespace std;

// ==========================================================================================================
// Public methods
// ==========================================================================================================
ElectionHandler :: ElectionHandler(ServerSocketWrapper socket, vector<SocketDescriptor> *candidates) {
    this->waitingFor = ELECTION;
    this->candidates = candidates;
    this->socket = socket;

    this->setTimeout();
}

void ElectionHandler :: receivePacket() {
    Packet *packet;

    while (true) {

        for (auto candidate : *(this->candidates)) {
            packet = this->socket.receivePacketFromClient(candidate, 1);
            this->handlePacket(candidate, packet);
        }

    }
}

void ElectionHandler :: notifyLiveness() {
    Packet imAlivePacket;
    imAlivePacket.command = IM_ALIVE;

    while (true) {

        for (auto candidate : *(this->candidates))
            this->socket.sendPacketToClient(candidate, &imAlivePacket);

        this->wait(MIN_TIMEOUT);
    }
}

void ElectionHandler :: answer(SocketDescriptor socket) {
    Packet answerPacket;
    answerPacket.command = ANSWER;

    this->socket.sendPacketToClient(socket, &answerPacket);
}

void ElectionHandler :: startElection() {
    if (this->waitingFor == ANSWER) return;

    vector<SocketDescriptor> eligible = this->getEligible();

    Packet electionPacket;
    electionPacket.command = ELECTION;

    for (auto eligible : eligible)
        this->socket.sendPacketToClient(eligible, &electionPacket);

    this->wins = 0;
    this->waitForAnswer();
}

void ElectionHandler :: communicateWin() {
    Packet coordinatorPacket;
    coordinatorPacket.command = COORDINATOR;

    for (auto candidate : *(this->candidates))
        this->socket.sendPacketToClient(candidate, &coordinatorPacket);
}

// ==========================================================================================================
// Private methods
// ==========================================================================================================
vector<SocketDescriptor> ElectionHandler :: getEligible() {
    vector<SocketDescriptor> eligible;
    SocketDescriptor thisCandidate = this->socket.getSocketDescriptor();

    for (auto candidate : *(this->candidates)) {
        if (candidate > thisCandidate)
            eligible.push_back(candidate);
    }

    return eligible;
}

void ElectionHandler :: handlePacket(SocketDescriptor socket, Packet *packet) {
    if (packet == NULL) {

        switch (this->waitingFor) {
            case ANSWER:
                this->wins++;

                if (this->wins == (*(this->candidates)).size()) {
                    this->communicateWin();
                    this->waitForElection();
                    // TODO: config new coordinator
                }

                break;

            case ELECTION:
            case COORDINATOR:
                this->startElection();
                break;
        }

    } else {

        switch (packet->command) {
            case ANSWER:
                this->waitForCoordinator();
                break;

            case ELECTION:
                this->answer(socket);
                this->startElection();
                break;

            case COORDINATOR:
                // TODO: do stuff to change coordination
                this->waitForElection();
                break;
        }
    }
}

void ElectionHandler ::setTimeout() {
    this->timeout = MAX_TIMEOUT;
}

void ElectionHandler :: wait() {
    this->wait(this->timeout);
}

void ElectionHandler :: wait(unsigned int timeout) {
    sleep(timeout);
}

void ElectionHandler :: waitForAnswer() {
    this->waitingFor = ANSWER;
    this->wait();
}

void ElectionHandler :: waitForElection() {
    this->waitingFor = ELECTION;
}

void ElectionHandler :: waitForCoordinator() {
    this->waitingFor = COORDINATOR;
    this->wait();
}
