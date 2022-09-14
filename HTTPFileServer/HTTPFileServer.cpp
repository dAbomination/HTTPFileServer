#include "include/http_server/http_server.h"

#include <iostream>

static const int CONNECTION_PORT = 9080;

int main(int argc, char* argv[]) {    
    Port port(CONNECTION_PORT);

    int thr = 1;
    Address addr(Ipv4::any(), port);

    http_server server(addr);

    server.init(thr);
    server.start();
}