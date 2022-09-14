#include "http_server.h"

inline void http_server::WronImportsRequest(Http::ResponseWriter& resp) {
    resp.send(Http::Code::Bad_Request, "Validation Failed");
}

inline void http_server::NonExistentId(Http::ResponseWriter& resp) {
    resp.send(Http::Code::Not_Found, "Item not found");
}

void http_server::ImportsRequest(const Rest::Request& req, Http::ResponseWriter resp){
    rapidjson::Document doc;
    doc.Parse(req.body().c_str());
    
    if(!doc.HasMember("items") || !doc.HasMember("updateDate") ){
        WronImportsRequest(resp);
        return;
    }
    // Check if updateTime is valid
    std::string update_date = doc["updateDate"].GetString();
    if( !UpdateDateIsValid(update_date) ) {
        WronImportsRequest(resp);
        return;
    }

    const rapidjson::Value& items = doc["items"];
    // All data for sql request
    std::vector<item_imports> sql_items_imports;
    std::vector<item_imports> sql_items_updates;

    for(size_t i = 0; i < items.Size(); ++i){
        // id cant be null, and if it is a FILE, size cant be null
        if(items[i]["id"].IsNull() 
            || (items[i]["type"].GetString() == "FILE" &&  items[i]["size"].IsNull()) ){
            WronImportsRequest(resp);
            return;
        }      
                
        std::optional<std::string> parentId = std::nullopt;
        if( items[i].HasMember("parentId") && !items[i]["parentId"].IsNull()) {
            parentId = items[i]["parentId"].GetString();
        }
        std::optional<int64_t> item_size = std::nullopt;
        if( items[i].HasMember("size") && !items[i]["size"].IsNull()) {
            item_size = items[i]["size"].GetInt64();            
        }
        std::optional<std::string> item_url = std::nullopt;
        if( items[i].HasMember("url") && !items[i]["url"].IsNull()) {
            item_url = items[i]["url"].GetString();            
        }

        item_imports temp_import = {
            items[i]["id"].GetString(),
            item_url,
            parentId,
            str_to_item_type.at(items[i]["type"].GetString()),
            item_size,
            update_date
        };

        // Check if this id already esists
        // if id already exist, just update it
        if( ids_to_children_.count(items[i]["id"].GetString()) > 0 ) {
            sql_items_updates.push_back(std::move(temp_import));            
        }
        else {
            sql_items_imports.push_back(std::move(temp_import));
        }
        
    }
    // First add elements and then update
    db_.InsertItem(sql_items_imports);  
    db_.UpdateItem(sql_items_updates);  

    db_.GetIds(ids_to_children_);

    resp.send(Http::Code::Ok);  
}

void http_server::GetNodesRequest(const Rest::Request& req, Http::ResponseWriter resp) {
    // Get id and check if id as parameter was transferred
    std::string id;
    if(!req.hasParam(":id")){
        WronImportsRequest(resp);
        return;
    }
    else {
        id = req.param(":id").as<std::string>();
    }
    // Check if such Id exists
    if( ids_to_children_.count(id) == 0 ){
        NonExistentId(resp);
        return;
    }
    // Json answer 
    rapidjson::Document doc;
    rapidjson::Value json_answer(rapidjson::kObjectType);
    int64_t temp = 0;
    GetItemTreeInformation(id, json_answer, doc, temp);    
    doc.CopyFrom(json_answer, doc.GetAllocator());

    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    doc.Accept(writer);
    
    resp.send(Http::Code::Ok, sb.GetString());
}

void http_server::DeleteRequest(const Rest::Request& req, Http::ResponseWriter resp) {
    // Get id and check if id as parameter was transferred
    std::string id;    
    if(!req.hasParam(":id")){
        WronImportsRequest(resp);
        return;
    }
    else {
        id = req.param(":id").as<std::string>();
    }
    // std::cout << id << std::endl;
    // Check if such Id exists
    if( ids_to_children_.count(id) == 0 ) {
        NonExistentId(resp);
        return;
    }  

    DeleteItemId(id);
    resp.send(Http::Code::Ok);
}

void http_server::GetItemTreeInformation(
    const std::string& id,
    rapidjson::Value& json_info, 
    rapidjson::Document& doc,
    int64_t& folder_size) {

    item_imports item_info = std::move(db_.GetItemInfo(id));
    // First adding item with id info    r     
    // Add file info to json answer
    json_info.AddMember("id",
        rapidjson::Value(item_info.id.c_str(), doc.GetAllocator()).Move(),
        doc.GetAllocator());
    json_info.AddMember("url",
        item_info.url.has_value() ? 
        rapidjson::Value(item_info.url.value().c_str(), doc.GetAllocator()).Move() :
        rapidjson::Value().Move(),
        doc.GetAllocator()); 
    json_info.AddMember("parentId",
        item_info.parentId.has_value() ? 
        rapidjson::Value(item_info.parentId.value().c_str(), doc.GetAllocator()).Move() :
        rapidjson::Value().Move(),
        doc.GetAllocator());
    json_info.AddMember("type",
        rapidjson::Value(item_type_to_str.at(item_info.type).c_str(), doc.GetAllocator()).Move(),
        doc.GetAllocator());    

    ConvertTimeStr(item_info.updateDate);    
    json_info.AddMember("date",
        rapidjson::Value(item_info.updateDate.c_str(), doc.GetAllocator()).Move(),
        doc.GetAllocator());

    // For File field "children" = null
    if( item_info.type == item_type::FILE_ITEM ) {
        // Add size
        json_info.AddMember("size",
        rapidjson::Value().SetInt64(item_info.size.value()).Move(),
        doc.GetAllocator());    
        folder_size += item_info.size.value();    

        json_info.AddMember("children", rapidjson::Value().Move(), doc.GetAllocator());
    }  
    // For empty folder size = 0;
    else if( item_info.type == item_type::FOLDER_ITEM ) {
        rapidjson::Value children_array(rapidjson::kArrayType);        
        // Folder has children
        if(ids_to_children_.at(id).size() > 0) {            
            // Every child info adding to "children" array
            int64_t current_folder_size = 0;
            for(const auto& child_id : ids_to_children_.at(id)) {
                rapidjson::Value child_json_info(rapidjson::kObjectType);
                GetItemTreeInformation(child_id, child_json_info, doc, current_folder_size);
                
                children_array.PushBack(child_json_info, doc.GetAllocator());
            }
            json_info.AddMember("size",
                rapidjson::Value().SetInt64(current_folder_size).Move(),
                doc.GetAllocator());
            folder_size += current_folder_size;
        }
        else {
            // No children - empty array            
            json_info.AddMember("size",
                rapidjson::Value().SetInt64(0).Move(),
                doc.GetAllocator());             
        }
        json_info.AddMember("children", children_array.Move(), doc.GetAllocator());
    }    
}

void http_server::DeleteItemId(const std::string& id) {
    // if this is a folder, we have to delete all internal items
    if(!ids_to_children_[id].empty())   {
        for(const auto& child : ids_to_children_[id]) {
            DeleteItemId(child);
        }        
    }    
    db_.DeleteItem(id);                
    
    // delete from ids_to_children_
    ids_to_children_.erase(id);
}

http_server::http_server(Address addr)
    : http_endpoint_(std::make_shared<Http::Endpoint>(addr)) {
}

void http_server::init(size_t thr = 1) {
    auto opts = Http::Endpoint::options().threads(static_cast<int>(thr));
    http_endpoint_->init(opts);

    db_.GetIds(ids_to_children_);
}

void http_server::start() {
    /* Routes will be set up here */
    Routes::Post(router_, "/imports", Routes::bind(&http_server::ImportsRequest, this));
    Routes::Get(router_, "/nodes/:id?", Routes::bind(&http_server::GetNodesRequest, this));
    Routes::Delete(router_, "/delete/:id?", Routes::bind(&http_server::DeleteRequest, this));
    
    // Set the handler to our router    
    http_endpoint_->setHandler(router_.handler());    
    try { 
        http_endpoint_->serve();  // start the server  
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl; 
    } 
}

void http_server::stop() {
    http_endpoint_->shutdown();    
}

bool http_server::UpdateDateIsValid(const std::string& s) {
    using namespace date;
    std::istringstream in{s};
    date::sys_time<std::chrono::milliseconds> tp;
    in >> parse("%FT%TZ", tp);
    return !in.fail();
}

void http_server::ConvertTimeStr(std::string& time_str) {
    size_t space_pos = time_str.find(' ');
    time_str[space_pos] = 'T';
    time_str += "Z";
}