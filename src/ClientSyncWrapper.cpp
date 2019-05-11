#include "../include/ClientSyncWrapper.hpp"
#include <sys/inotify.h>
#include <iostream>
#include <string>

using namespace std;

#define EVENT_SIZE  (sizeof (struct inotify_event))

#define BUF_LEN (EVENT_SIZE + 4096)

static void dealWithEvent(struct inotify_event *event){
    if (event->len != 32){
        cout << "Mask: " << event->mask << endl;
        cout << "Len: " << event->len << endl;
        cout << "Name: " << event->name << endl;
        string swp = "swp";
        string name = event->name;
        if (name.find(swp) != std::string::npos){
            cout << "eh swp"<<endl;
        }
        
    }


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
