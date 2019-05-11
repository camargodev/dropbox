#include "../include/ClientSyncWrapper.hpp"
#include <sys/inotify.h>
#include <iostream>

using namespace std;

#define EVENT_SIZE  (sizeof (struct inotify_event))

/* reasonable guess as to size of 1024 events */
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

static void displayInotifyEvent(struct inotify_event *i)
{
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
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
                cout << "Mask: " << event->mask << endl;
                cout << "Len: " << event->len << endl;
                cout << "Name: " << event->name << endl;
                i += event->len + EVENT_SIZE;

        }
    }

//    cout << "Watcher: " << watcher << endl;
}
