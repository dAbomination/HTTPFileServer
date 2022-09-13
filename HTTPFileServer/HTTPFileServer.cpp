#include "include/rapidjson/document.h"
#include "include/http_server/http_server.h"

#include <iostream>

int main(int argc, char* argv[]) {    
    Port port(9080);

    int thr = 1;
    Address addr(Ipv4::any(), port);

    //std::cout << "Cores = " << hardware_concurrency() << std::endl;
    //std::cout << "Using " << thr << " threads" << std::endl;

    http_server server(addr);

    server.init(thr);
    server.start();
}