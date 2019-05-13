#include "../include/ClientSyncWrapper.hpp"
#include <sys/inotify.h>
#include <iostream>
#include <string>

using namespace std;

#define EVENT_SIZE  (sizeof (struct inotify_event))

#define BUF_LEN (EVENT_SIZE + 4096)
int notify_count;
static void dealWithEvent(struct inotify_event *event){
    string name = event->name;
    if ((event->len == 32) || (name.find(".") == 0)){
        return;
    }
    cout << "========="<<notify_count<<"==========="<<endl;
    notify_count++;
    if(event->mask & IN_CLOSE_WRITE){
        cout << "EDITOU um arquivo" <<endl;
    }
    if(event->mask & IN_MOVED_FROM){
        cout << "DELETOU um arquivo" <<endl;
    }
    if(event->mask & IN_MOVED_TO){
        cout << "CRIOU um arquivo" <<endl;
    }
    cout << "Name: " << event->name << endl;
}

void ClientSyncWrapper::checkForUpdates() {
    int fd = inotify_init1(IN_NONBLOCK);
    int wd = inotify_add_watch(fd, "/home/lucio/monitora", IN_CLOSE_WRITE | IN_MOVED_FROM | IN_MOVED_TO);
    char *buffer;

    int size_read, i;
    while(true) {
        i = 0;
        size_read = read(fd, buffer, BUF_LEN);
        if(size_read > 0) {

            inotify_event* event = (struct inotify_event*) buffer;
            dealWithEvent(event);
            i += event->len + EVENT_SIZE;

        }
    }

//    cout << "Watcher: " << watcher << endl;
}
