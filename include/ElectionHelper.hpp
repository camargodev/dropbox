#ifndef ELECTION_HELPER
#define ELECTION_HELPER

#include <vector>
#include "ReplicationHelper.hpp"

class ElectionHelper {

public:
    const static int TIMEOUT_FOR_ANSWER = 5;
    const static int TIMEOUT_FOR_COORDINATOR = 5;

    ElectionHelper();
    bool isOtherMirrorMorePrioritary(Mirror me, Mirror otherMirror);
    vector<Mirror> getMirrorsWithHighestPrio(Mirror me, vector<Mirror> otherMirrors);
    bool hasAlreadyStartedElection();
    void setElectionAsStarted();
    void confirmAnswerReceived();
    bool hasReceivedAnswer();
    Clock getClockWhenElectionStarted();
    void reset();

private:
    bool electionAlreadyStarted = false;
    bool receivedAnswer = false;
    Clock clockWhenElectionStarted;
};

#endif 
