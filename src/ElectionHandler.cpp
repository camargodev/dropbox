#include <vector>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "../include/Packet.hpp"
#include "../include/Command.hpp"
#include "../include/ElectionHandler.hpp"
#include "../include/ServerSocketWrapper.hpp"

using namespace std;

// ==========================================================================================================
// Public methods
// ==========================================================================================================
ElectionHandler :: ElectionHandler(ServerSocketWrapper serverSocket, SocketDescriptor primarySocket, vector<Candidate> *candidates) {
    this->waitingFor = ELECTION;
    this->candidates = candidates;
    this->serverSocket = serverSocket;
    this->primarySocket = primarySocket;

    this->setTimeout();
}

void ElectionHandler :: receivePacket() {
    Packet *packet;

    while (true) {
        this->waitMaxTimeout();

        for (auto candidate : *(this->candidates)) {
            packet = this->serverSocket.receivePacketFromClient(candidate, 1);
            this->handlePacket(candidate, packet);
        }
    }
}

void ElectionHandler :: notifyLiveness() {
    Packet imAlivePacket;
    imAlivePacket.command = IM_ALIVE;

    while (true) {

        for (auto candidate : *(this->candidates))
            this->serverSocket.sendPacketToClient(candidate, &imAlivePacket);

        this->waitMinTimeout();
    }
}

void ElectionHandler :: receiveLiveness() {
    Packet *packet;

    while (true) {
        this->waitMinTimeout();

        if(this->waitingFor == ELECTION) {
            packet = this->serverSocket.receivePacketFromClient(this->primarySocket, 1);
            if (packet == NULL) this->startElection();
        }
    }
}

// ==========================================================================================================
// Private methods
// ==========================================================================================================
void ElectionHandler :: startElection() {
    if (this->waitingFor == ANSWER) return;

    vector<Candidate> eligible = this->getEligible();

    Packet electionPacket;
    electionPacket.command = ELECTION;

    for (auto eligible : eligible)
        this->serverSocket.sendPacketToClient(eligible, &electionPacket);

    this->wins = 0;
    this->waitForAnswer();
}

void ElectionHandler :: communicateWin() {
    Packet coordinatorPacket;
    coordinatorPacket.command = COORDINATOR;

    for (auto candidate : *(this->candidates))
        this->serverSocket.sendPacketToClient(candidate, &coordinatorPacket);
}

void ElectionHandler :: answer(SocketDescriptor socket) {
    Packet answerPacket;
    answerPacket.command = ANSWER;

    this->serverSocket.sendPacketToClient(socket, &answerPacket);
}

vector<Candidate> ElectionHandler :: getEligible() {
    vector<Candidate> eligible;
    SocketDescriptor thisCandidate = this->serverSocket.getSocketDescriptor();

    for (auto candidate : *(this->candidates)) {
        if (candidate > thisCandidate)
            eligible.push_back(candidate);
    }

    return eligible;
}

ElectionResult ElectionHandler :: handlePacket(SocketDescriptor socket, Packet *packet) {
    if (packet == NULL) {

        switch (this->waitingFor) {
            case ANSWER:
                this->wins++;

                if (this->wins == (*(this->candidates)).size()) {
                    this->communicateWin();
                    this->waitForElection();
                    identifyAsCoordinator();
                }

                break;

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
                setCoordinator(socket);
                this->waitForElection();
                break;

            case TIMEOUT_TEST:
                this->serverSocket.sendPacketToClient(candidate, packet);
                break;
        }
    }
}

void ElectionHandler :: setTimeout() {
    Packet timeoutPacket;
    timeoutPacket.command = TIMEOUT_TEST;

    time_t start, stop, diff, max_diff = 0, min_diff = 0;

    for(int i = 0; i < 3; i++) {
        for (auto candidate : *(this->candidates)) {
            start = time(0);
            this->serverSocket.sendPacketToClient(candidate, &timeoutPacket);
            this->serverSocket.receivePacketFromClient(candidate);
            stop = time(0);

            diff = ceil(stop - start);

            if (diff > max_diff)
                max_diff = diff;
            else if (diff < min_diff)
                min_diff = diff;
        }
    }

    this->max_timeout = max_diff + 2;
    this->min_timeout = min_diff + 1;
}

void ElectionHandler ::waitMaxTimeout(){
    this->wait(this->max_timeout);
}

void ElectionHandler ::waitMinTimeout(){
    this->wait(this->min_timeout);
}

void ElectionHandler :: wait(unsigned int timeout) {
    sleep(timeout);
}

void ElectionHandler :: waitForAnswer() {
    this->waitingFor = ANSWER;
    this->waitMaxTimeout();
}

void ElectionHandler :: waitForElection() {
    this->waitingFor = ELECTION;
}

void ElectionHandler :: waitForCoordinator() {
    this->waitingFor = COORDINATOR;
    this->waitMaxTimeout();
}
