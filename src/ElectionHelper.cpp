#include "../include/ElectionHelper.hpp"

using namespace std;

bool ElectionHelper :: isOtherMirrorMorePrioritary(Mirror me, Mirror other) {
    int comparison = strcmp(me.ip, other.ip);
    if (comparison < 0) 
        return true;
    if (comparison == 0 && me.port < other.port) 
        return true;
    return false;
}

vector<Mirror> ElectionHelper :: getMirrorsWithHighestPrio(Mirror me, vector<Mirror> otherMirrors) {
    vector<Mirror> mirrorsWithMorePriority;
    for (auto otherMirror : otherMirrors) 
        if (isOtherMirrorMorePrioritary(me, otherMirror))
            mirrorsWithMorePriority.push_back(otherMirror);
    return mirrorsWithMorePriority;
}

bool ElectionHelper :: hasAlreadyStartedElection() {
    return electionAlreadyStarted;
}

void ElectionHelper :: setElectionAsStarted() {
    electionAlreadyStarted = true;
}