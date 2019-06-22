#include "../include/Notifier.hpp"
#include <iostream>

void Notifier :: setDirectory(string dirName) {
    printf("I will watch directory %s\n", dirName.c_str());
    this->dirName = dirName;
}

bool Notifier :: shouldIgnoreEvent(Event event) {
    string name = event->name;
    return (event->len == 32) || (name.find(".") == 0);
}

EventType Notifier :: getEventType(Event event) {
    if (event->mask & IN_CLOSE_WRITE)
        return CREATE;
    if (event->mask & IN_MOVED_TO)
        return EDIT;
    if (event->mask & IN_MOVED_FROM)
        return DELETE;
    return NO_ACTION;
}

Action Notifier :: getActionForEvent(Event event){
    if (shouldIgnoreEvent(event))
        return Action(NO_ACTION);
    EventType type = getEventType(event);
    return Action(type, string(event->name));
}

void Notifier :: startWatching() {
    this->notifierDescriptor = inotify_init1(IN_NONBLOCK);
    this->watcherDescriptor = inotify_add_watch(notifierDescriptor, dirName.c_str(), IN_CLOSE_WRITE | IN_MOVED_FROM | IN_MOVED_TO);
}

void Notifier :: stopWatching() {
    inotify_rm_watch(notifierDescriptor, watcherDescriptor);
}

Verification Notifier :: verificate() {
    Verification verification;
    int sizeRead = read(this->notifierDescriptor, verification.buffer, BUF_LEN);
    verification.hasChanges = sizeRead > 0;
    return verification;
}

Event Notifier :: readEventFromBuffer(char *buffer[BUF_LEN]) {
    return (struct inotify_event*) buffer;
}

Action Notifier :: getListenedAction() {
    Verification verification = verificate();
    if (!verification.hasChanges)
        return Action(NO_ACTION);
    Event event = readEventFromBuffer(verification.buffer);
    return getActionForEvent(event);
}