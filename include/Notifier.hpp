#ifndef NOTIFIER_HPP
#define NOTIFIER_HPP

#include <sys/inotify.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

using namespace std;

#define EVENT_SIZE  (sizeof (struct inotify_event))
#define BUF_LEN (EVENT_SIZE + 4096)
using Event = inotify_event*;
using EventType = int;

struct Verification {
    bool hasChanges;
    char *buffer[BUF_LEN];
};

struct Action {
    EventType type;
    string filename;

    Action(EventType type) {
        this->type = type;
    }

    Action(EventType type, string filename) {
        this->type = type;
        this->filename = filename;
    }
};

class Notifier {
public:
    const static EventType CREATE    = 1010;
    const static EventType DELETE    = 1011;
    const static EventType EDIT      = 1100;
    const static EventType NO_ACTION = -100;

    Action getListenedAction();
    void startWatching();
    void stopWatching();
    void setDirectory(string dirName);
private:
    int notifierDescriptor;
    int watcherDescriptor;
    string dirName;
    bool shouldIgnoreEvent(Event event);
    EventType getEventType(Event event);
    Verification verificate();
    Event readEventFromBuffer(char *buffer[BUF_LEN]);
    Action getActionForEvent(Event event);
};
#endif