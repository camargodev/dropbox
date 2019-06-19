#ifndef ADD_GETTER_HANDLER_HPP
#define ADD_GETTER_HANDLER_HPP

#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <stdlib.h>
#include <vector>

using namespace std;
using Address = struct ifaddrs*;

class AddressGetter {
public:
    char* getIP();
private:
    Address getFirstAddress();    
    void freeAddressList(Address firstAddress);
    bool isValidAddress(Address address);
    vector<Address> getMachineAddresses();
    bool isIPV4(Address address);
    bool isNotLocalhost(Address address);
    char* extractIP(Address address);
};


#endif