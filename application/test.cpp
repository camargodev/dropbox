#include "../include/ClientSyncWrapper.hpp"

int main() {
    ClientSyncWrapper* client = new ClientSyncWrapper();

    client->checkForUpdates();
    return 0;
}
