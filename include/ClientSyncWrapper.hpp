#ifndef DROPBOX_CLIENTSYNCWRAPPER_H
#define DROPBOX_CLIENTSYNCWRAPPER_H

#include "ClientSocketWrapper.hpp"

class ClientSyncWrapper {

    public:
        void checkForUpdates();
        void sendUpdate();
        void receiveUpdate();

    private:
        ClientSocketWrapper socket;
};


#endif //DROPBOX_CLIENTSYNCWRAPPER_H
