#include "db_adapter.h"

// Connect to database
db_adapter::db_adapter() {    
    try {     
        db_con_ = std::make_unique<pqxx::connection>("\
            dbname = " + DB_NAME + "\
            user = " + DB_USER + "\
            password = " + DB_PASSWORD + "\
            hostaddr = " + DB_HOSTADDR + "\
            port = " + DB_PORT );
        //std::cout << "Connected to DB: " + DB_NAME << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void db_adapter::GetIds(common::FileDependencies& paths) { 
    std::string sql = "SELECT id, parentId FROM " + FILES_DATA_TABLE_NAME + "";
    pqxx::result bd_request_result = std::move(ExecuteRequest(sql));
    
    for(auto const& row : bd_request_result) {
        if(!row.at(1).is_null()) {
            paths[row.at(1).c_str()].insert(row.at(0).c_str());
            paths[row.at(0).c_str()];            
        }        
        // if item has no children - empty vector
        else {
            paths[row.at(0).c_str()];            
        }
    }      
}

common::item_imports db_adapter::GetItemInfo(const std::string& id) {    
    std::string sql = "SELECT * \
        FROM " + FILES_DATA_TABLE_NAME + " \
        WHERE id = '" + id + "'";        
    // The check for existence was earlier,
    // which means that with the same id there is only 1 result
    pqxx::result bd_request_result = std::move(ExecuteRequest(sql));

    std::optional<std::string> url = std::nullopt;
    if(!bd_request_result.at(0).at(1).is_null()) {
        url = bd_request_result.at(0).at(1).c_str();
    }
    std::optional<std::string> parentId = std::nullopt;
    if(!bd_request_result.at(0).at(2).is_null()) {
        parentId = bd_request_result.at(0).at(2).c_str();
    }
    std::optional<int64_t> size = std::nullopt;
    if(!bd_request_result.at(0).at(4).is_null()) {
        size = bd_request_result.at(0).at(4).as<int64_t>();
    }
    
    return {
        id,
        url,
        parentId,
        str_to_item_type.at(bd_request_result.at(0).at(3).c_str()),
        size,
        bd_request_result.at(0).at(5).as<std::string>()
    };
}

std::unordered_set<std::string> db_adapter::GetUpdatedIds(
    const std::string& from_date,
    const std::string& to_date) {
    
    std::string sql = "SELECT id FROM updates WHERE \
        updatedate >= '" + from_date +
        "' and updatedate <= '" + to_date + "'";
    pqxx::result bd_request_result = std::move(ExecuteRequest(sql));

    std::unordered_set<std::string> result;
    for(auto const& row : bd_request_result) {
        result.insert(row.at(0).as<std::string>());
    }  

    return result;
}

void db_adapter::InsertItem(const common::item_imports& import_item) {    

    std::string parentId = "null";
    if(import_item.parentId.has_value()) {
        parentId = "'" + import_item.parentId.value() + "'";
    }    
    std::string size = "null";
    if(import_item.size.has_value()) {
        size = std::to_string(import_item.size.value()); 
    }
    std::string url = "null";
    if(import_item.url.has_value()) {
        url = "'" + import_item.url.value() + "'"; 
    }

    std::string sql = "INSERT INTO " + FILES_DATA_TABLE_NAME + " \
        VALUES('" + import_item.id + "', " +
        url + " , " + 
        parentId + ", '" +
        item_type_to_str.at(import_item.type) + "'," +
        size + " , '" +
        import_item.updateDate + "')";

    ExecuteRequest(sql);
}

void db_adapter::InsertItem(const std::vector<common::item_imports>& import_items) {
    for(const auto& item : import_items) {
        InsertItem(item);
    }
}

void db_adapter::UpdateItem(const common::item_imports& update_item) {    

    std::string parentId = "null";
    if(update_item.parentId.has_value()){
        parentId = "'" + update_item.parentId.value() + "'";
    }    
    std::string size = "null";
    if(update_item.size.has_value()){
        size = std::to_string(update_item.size.value());
    }
    std::string url = "null";
    if(update_item.url.has_value()) {
        url = "'" + update_item.url.value() + "'"; 
    }

    std::string sql = "UPDATE " + FILES_DATA_TABLE_NAME + " \
        SET url = " + url + " , " +
        "parentId = " + parentId + ", " +
        "type = '" + item_type_to_str.at(update_item.type) + "', " +
        "size = " + size + ", " +
        "updateDate = '" + update_item.updateDate + "' " +
        "WHERE id = '" + update_item.id + "'";

    ExecuteRequest(sql);
}

void db_adapter::UpdateItem(const std::vector<common::item_imports>& update_items) {
    for(const auto& item : update_items) {
        UpdateItem(item);
    }
}

void db_adapter::InsertUpdates(const common::update_date_data& data) {    
    std::string sql = "INSERT INTO " + FILES_UPDATES_TABLE_NAME + " \
        VALUES('" + data.updateDate + "', '" +
        data.id + "' , '" + 
        data.raw_json_data + "')";

    ExecuteRequest(sql);
}

void db_adapter::InsertUpdates(const std::vector<common::update_date_data>& data) {
    for(const auto& single_data : data) {
        InsertUpdates(single_data);
    }
}

void db_adapter::DeleteItem(const std::string& id) {
    std::string sql = "DELETE FROM " + FILES_DATA_TABLE_NAME + " \
        WHERE id = '" + id + "'" ;
    ExecuteRequest(sql);
}

void db_adapter::DeleteUpdates(const std::string& id) {    
    std::string sql = "DELETE FROM " + FILES_UPDATES_TABLE_NAME + " \
        WHERE id = '" + id + "'" ;
    ExecuteRequest(sql);
}

std::vector<common::update_date_data> db_adapter::GetItemHistory(
    const std::string& id,
    const std::string& date_start,
    const std::string& date_end) {

    std::string sql = "SELECT * FROM updates WHERE \
        updatedate >= '" + date_start +
        "' and updatedate < '" + date_end +
        "' and id = '" + id + "'";
    pqxx::result bd_request_result = std::move(ExecuteRequest(sql));

    std::vector<common::update_date_data> result;
    for(auto const& row : bd_request_result) {
        result.push_back({
            row.at(0).as<std::string>(),
            row.at(1).as<std::string>(),
            row.at(2).as<std::string>()
        });
    } 
    return result; 
}

pqxx::result db_adapter::ExecuteRequest(const std::string& sql_req) {
    pqxx::work tx(*db_con_);
    pqxx::result bd_request_result = tx.exec(sql_req);    
    tx.commit();
    return bd_request_result;
}