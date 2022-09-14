#pragma once

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <unordered_map>

#include "date/date.h"
#include "../include/rapidjson/document.h" 
#include "../include/rapidjson/stringbuffer.h"
#include "../include/rapidjson/writer.h"
#include "db_adapter/db_adapter.h"


using namespace Pistache;
using namespace Pistache::Rest;

class http_server {
public:    
    // server initialization 
    explicit http_server(Address addr);

    void init(size_t thr);
    
    void start();
    void stop();
private:  
    inline void WronImportsRequest(Http::ResponseWriter& resp);
    inline void NonExistentId(Http::ResponseWriter& resp);

    // Returns true if updateDate is correct
    bool UpdateDateIsValid(const std::string& s);
    // Convert from YY:MM:DD HH:MM:SS to YY:MM:DDTHH:MM:SSZ
    void ConvertTimeStr(std::string& time_str);

    // Requests
    void ImportsRequest(const Rest::Request& req, Http::ResponseWriter resp);
    void DeleteRequest(const Rest::Request& req, Http::ResponseWriter resp);
    void GetNodesRequest(const Rest::Request& req, Http::ResponseWriter resp);      

    // Whether it's a folder or a file
    void DeleteItemId(const std::string& id);
    // For folder return all the internal items information    
    void GetItemTreeInformation(
        const std::string& id, 
        rapidjson::Value& json_info,
        rapidjson::Document& doc,
        int64_t& fosder_size);
    
    std::shared_ptr<Http::Endpoint> http_endpoint_;
    Rest::Router router_;    
    db_adapter db_;

    FileDependencies ids_to_children_;
};