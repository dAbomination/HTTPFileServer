#include "include/rapidjson/document.h"
#include "include/http_server/http_server.h"

#include <iostream>
// void hello(const Rest::Request& request, Http::ResponseWriter response) {
//     response.send(Http::Code::Ok, "world!");
// }

// void echo_get(const Rest::Request& req, Http::ResponseWriter resp) {
//     // get the parameter value, default to an warning message if it's not available
//     std::string text = req.hasParam(":text") ?  // check if parameter is included
//                        req.param(":text").as<std::string>() : // if so set as text value
//                        "No parameter supplied.";  // otherwise return warning
//     resp.send(Http::Code::Ok, text);  // return a response from our server
// }

// void echo(const Rest::Request& req, Http::ResponseWriter resp) {
//     rapidjson::Document doc;
//     doc.Parse(req.body().c_str());
//     std::string responseString = doc.HasMember("text") ? doc["text"].GetString() : "No text parameter supplied in JSON:\n" + req.body();
//     resp.send(Http::Code::Ok, "Message received!");
// }


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