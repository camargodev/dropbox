#include "../include/AddressGetter.hpp"
#include "string"

Address AddressGetter :: getFirstAddress() {
    Address address = NULL;
    getifaddrs(&address);
    return address;
}

void AddressGetter :: freeAddressList(Address firstAddress) {
    if (firstAddress != NULL) 
        freeifaddrs(firstAddress);
}

bool AddressGetter :: isValidAddress(Address address) {
    return address->ifa_addr;
}

vector<Address> AddressGetter :: getMachineAddresses() {
    vector<Address> addresses;
    Address firstAddress = getFirstAddress();

    for (Address address = firstAddress; address != NULL; address = address->ifa_next)
        if (isValidAddress(address)) 
            addresses.push_back(address);       

    freeAddressList(firstAddress);
    return addresses;
}

bool AddressGetter :: isIPV4(Address address) {
    return address->ifa_addr->sa_family == AF_INET;
}

bool AddressGetter :: isNotLocalhost(Address address) {
    return strcmp(address->ifa_name, "lo") != 0;
}

char* AddressGetter :: extractIP(Address address) {
    char* addressBuffer = new char[INET_ADDRSTRLEN]();
    inet_ntop(AF_INET, &((struct sockaddr_in *)address->ifa_addr)->sin_addr, addressBuffer, INET_ADDRSTRLEN); 
    return addressBuffer;
}

char* AddressGetter :: getIP() {
    for (auto address : getMachineAddresses()) 
        if (isIPV4(address) && isNotLocalhost(address))
            return extractIP(address);
    return (char*) "localhost";
}