#include <iostream>

#include "http_server.h"

static const int CONNECTION_PORT = 80;

int main(int argc, char* argv[]) {    
    Port port(CONNECTION_PORT);

    int thr = 1;
    Address addr(Ipv4::any(), port);

    http_server server(addr);

    server.init(thr);
    server.start();
}