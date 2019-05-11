#include "../include/ClientSyncWrapper.hpp"
#include <sys/inotify.h>
#include <iostream>

using namespace std;

#define EVENT_SIZE  (sizeof (struct inotify_event))

/* reasonable guess as to size of 1024 events */
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

static void displayInotifyEvent(struct inotify_event *i)
{
    cout << "====================================" << endl;
    cout << "wd = " << i->wd << endl;
    if (i->cookie > 0)
        cout << "cookie = " << i->cookie << endl;

    cout << "mask = ";
    if (i->mask & IN_ACCESS)        cout << "IN_ACCESS ";
    if (i->mask & IN_ATTRIB)        cout << "IN_ATTRIB ";
    if (i->mask & IN_CLOSE_NOWRITE) cout << "IN_CLOSE_NOWRITE ";
    if (i->mask & IN_CLOSE_WRITE)   cout << "IN_CLOSE_WRITE ";
    if (i->mask & IN_CREATE)        cout << "IN_CREATE ";
    if (i->mask & IN_DELETE)        cout << "IN_DELETE ";
    if (i->mask & IN_DELETE_SELF)   cout << "IN_DELETE_SELF ";
    if (i->mask & IN_IGNORED)       cout << "IN_IGNORED ";
    if (i->mask & IN_ISDIR)         cout << "IN_ISDIR ";
    if (i->mask & IN_MODIFY)        cout << "IN_MODIFY ";
    if (i->mask & IN_MOVE_SELF)     cout << "IN_MOVE_SELF ";
    if (i->mask & IN_MOVED_FROM)    cout << "IN_MOVED_FROM ";
    if (i->mask & IN_MOVED_TO)      cout << "IN_MOVED_TO ";
    if (i->mask & IN_OPEN)          cout << "IN_OPEN ";
    if (i->mask & IN_Q_OVERFLOW)    cout << "IN_Q_OVERFLOW ";
    if (i->mask & IN_UNMOUNT)       cout << "IN_UNMOUNT ";
    else count << i->mask;
    count << endl;

    if (i->len > 0)
        cout << "name = " << i->name << endl;
}
void ClientSyncWrapper::checkForUpdates() {
    int fd = inotify_init1(IN_NONBLOCK);
    int wd = inotify_add_watch(fd, "/home/lucio/monitora", IN_MODIFY);
    char buffer[BUF_LEN];

    int size_read, i;
    while(true) {
        i = 0;
        size_read = read(fd, buffer, BUF_LEN);
        if(size_read > 0) {
                inotify_event* event = (struct inotify_event*) &buffer[i];
                displayInotifyEvent(event);
                i += event->len + EVENT_SIZE;
        }
    }

//    cout << "Watcher: " << watcher << endl;
}
