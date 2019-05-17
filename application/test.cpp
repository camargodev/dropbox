#include <sys/stat.h>
#include <iostream>

using namespace std;

int main() {
    if(mkdir("teste", 07777) == 0)
        cout << "OK\n";
    else
        cout << "NOT OK\n";
}
