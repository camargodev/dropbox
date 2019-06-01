#ifndef DROPBOX_ELECTIONHANDLER_HPP
#define DROPBOX_ELECTIONHANDLER_HPP

#include <vector>
#include "ServerSocketWrapper.hpp"

#define MAX_TIMEOUT 5
#define MIN_TIMEOUT 3

class ElectionHandler {

public:
    ElectionHandler(ServerSocketWrapper socket, vector<SocketDescriptor> *candidates);

    // Threadable
    void receivePacket();
    void notifyLiveness();

private:
    unsigned int wins;
    Command waitingFor;
    unsigned int timeout; // in seconds
    ServerSocketWrapper socket;
    vector<SocketDescriptor> *candidates;

    void startElection();
    void communicateWin();
    void answer(SocketDescriptor socket);

    vector<SocketDescriptor> getEligible();
    void handlePacket(SocketDescriptor socket, Packet *packet);

    void setTimeout();

    void wait();
    void waitForAnswer();
    void waitForElection();
    void waitForCoordinator();
    void wait(unsigned int timeout);
};

#endif //DROPBOX_ELECTIONHANDLER_HPP
