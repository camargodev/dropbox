#include "../include/ElectionHelper.hpp"
#include <semaphore.h>

using namespace std;
sem_t dealingWithMirrors;

ElectionHelper :: ElectionHelper() {
    sem_init(&dealingWithMirrors, 0, 1);
}

bool ElectionHelper :: isOtherMirrorMorePrioritary(Mirror me, Mirror other) {
    int comparison = strcmp(me.ip, other.ip);
    if (comparison < 0) 
        return true;
    if (comparison == 0 && me.port < other.port) 
        return true;
    return false;
}

vector<Mirror> ElectionHelper :: getMirrorsWithHighestPrio(Mirror me, vector<Mirror> otherMirrors) {
    sem_wait(&dealingWithMirrors);
    vector<Mirror> mirrorsWithMorePriority;
    for (auto otherMirror : otherMirrors) 
        if (isOtherMirrorMorePrioritary(me, otherMirror))
            mirrorsWithMorePriority.push_back(otherMirror);
    sem_post(&dealingWithMirrors);
    return mirrorsWithMorePriority;
}

bool ElectionHelper :: hasAlreadyStartedElection() {
    sem_wait(&dealingWithMirrors);
    auto hasStarted = electionAlreadyStarted; // just to be able to use semaphore
    sem_post(&dealingWithMirrors);
    return hasStarted;
}

void ElectionHelper :: setElectionAsStarted() {
    electionAlreadyStarted = true;
    clockWhenElectionStarted = clock();
}

bool ElectionHelper :: hasReceivedAnswer() {
    sem_wait(&dealingWithMirrors);
    auto hasReceived = receivedAnswer; // just to be able to use semaphore
    sem_post(&dealingWithMirrors);
    return hasReceived;
}

void ElectionHelper :: confirmAnswerReceived() {
    receivedAnswer = true;
}

Clock ElectionHelper :: getClockWhenElectionStarted() {
    return clockWhenElectionStarted;
}