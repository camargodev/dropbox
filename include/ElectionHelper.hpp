#ifndef ELECTION_HELPER
#define ELECTION_HELPER

#include <vector>
#include "ReplicationHelper.hpp"

class ElectionHelper {

public:
    bool isOtherMirrorMorePrioritary(Mirror me, Mirror otherMirror);
    vector<Mirror> getMirrorsWithHighestPrio(Mirror me, vector<Mirror> otherMirrors);
    bool hasAlreadyStartedElection();
    void setElectionAsStarted();

private:
    bool electionAlreadyStarted = false;
};

#endif 
