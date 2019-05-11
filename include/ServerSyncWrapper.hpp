#ifndef DROPBOX_SERVERSYNCWRAPPER_H
#define DROPBOX_SERVERSYNCWRAPPER_H

#include "ServerSocketWrapper.hpp"
#include "Packet.hpp"

class ServerSyncWrapper {

    public:
        void receivePacket(Packet* packet);

    private:
        ServerSocketWrapper socket;

        void notifyClients(Packet* packet);
};


#endif //DROPBOX_SERVERSYNCWRAPPER_H
