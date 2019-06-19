#ifndef DROPBOX_ELECTIONHANDLER_HPP
#define DROPBOX_ELECTIONHANDLER_HPP

#include <vector>
#include "ServerSocketWrapper.hpp"

using Candidate = SocketDescriptor;

extern identifyAsCoordinator();
extern setCoordinator(SocketDescriptor socketDescriptor);

class ElectionHandler {

public:
    ElectionHandler(ServerSocketWrapper serverSocket, SocketDescriptor primarySocket, vector<SocketDescriptor> *candidates);

    // Threadable
    void receivePacket();
    void notifyLiveness();
    void receiveLiveness();
    void setPrimarySocket(SocketDescriptor socketDescriptor);

private:
    unsigned int wins;
    Command waitingFor;
    unsigned int max_timeout; // in seconds
    unsigned int min_timeout; // in seconds
    SocketDescriptor primarySocket;
    ServerSocketWrapper serverSocket;
    vector<SocketDescriptor> *candidates;

    void startElection();
    void communicateWin();
    void answer(SocketDescriptor socket);

    vector<SocketDescriptor> getEligible();
    ElectionResult handlePacket(SocketDescriptor socket, Packet *packet);

    void setTimeout();

    void wait();
    void waitMaxTimeout();
    void waitMinTimeout();

    void waitForAnswer();
    void waitForElection();
    void waitForCoordinator();
    void wait(unsigned int timeout);
};

#endif //DROPBOX_ELECTIONHANDLER_HPP
