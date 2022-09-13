#include "db_adapter.h"

// Connect to database
db_adapter::db_adapter() {    
    try {     
        db_con_ = std::make_unique<pqxx::connection>("\
            dbname = http_server_disk\
            user = postgres\
            password = Ltybc1993\
            hostaddr = 127.0.0.1\
            port = 5432");
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void db_adapter::GetIds(FileDependencies& paths) {
    pqxx::work tx(*db_con_);

    std::string sql = "SELECT id, parentId \
        FROM files";        

    pqxx::result res = tx.exec(sql);
    
    for(auto const& row : res) {
        if(!row.at(1).is_null()) {
            paths[row.at(1).c_str()].push_back(row.at(0).c_str());
            paths[row.at(0).c_str()];            
        }        
        // if item has no children - empty vector
        else {
            paths[row.at(0).c_str()];            
        }
    }  
}

item_imports db_adapter::GetItemInfo(const std::string& id) {
    pqxx::work tx(*db_con_);

    std::string sql = "SELECT * \
        FROM files \
        WHERE id = '" + id + "'";        
    // The check for existence was earlier,
    // which means that with the same id there is only 1 result
    pqxx::result res = tx.exec(sql);

    std::optional<std::string> url = std::nullopt;
    if(!res.at(0).at(1).is_null()) {
        url = res.at(0).at(1).c_str();
    }
    std::optional<std::string> parentId = std::nullopt;
    if(!res.at(0).at(2).is_null()) {
        parentId = res.at(0).at(2).c_str();
    }
    std::optional<int64_t> size = std::nullopt;
    if(!res.at(0).at(4).is_null()) {
        size = res.at(0).at(4).as<int64_t>();
    }
    
    return {
        id,
        url,
        parentId,
        str_to_item_type.at(res.at(0).at(3).c_str()),
        size
    };
}

void db_adapter::InsertItem(const item_imports& import_item) {
    pqxx::work tx(*db_con_);

    std::string parentId = "null";
    if(import_item.parentId.has_value()) {
        parentId = "'" + import_item.parentId.value() + "'";
    }    
    std::string size = "null";
    if(import_item.size.has_value()) {
        size = "'" + std::to_string(import_item.size.value()) + "'"; 
    }
    std::string url = "null";
    if(import_item.url.has_value()) {
        url = "'" + import_item.url.value() + "'"; 
    }


    std::string sql = "INSERT INTO files \
        VALUES('" + import_item.id + "', " +
        url + " , " + 
        parentId + ", '" +
        item_type_to_str.at(import_item.type) + "'," +
        size + " )";

    tx.exec(sql);
    tx.commit();
}

void db_adapter::InsertItem(const std::vector<item_imports>& import_items) {
    for(const auto& item : import_items) {
        InsertItem(item);
    }
}

void db_adapter::UpdateItem(const item_imports& update_item) {
    pqxx::work tx(*db_con_);

    std::string parentId = "null";
    if(update_item.parentId.has_value()){
        parentId = "'" + update_item.parentId.value() + "'";
    }    
    std::string size = "null";
    if(update_item.size.has_value()){
        size = "'" + std::to_string(update_item.size.value()) + "'";
    }
    std::string url = "null";
    if(update_item.url.has_value()) {
        url = "'" + update_item.url.value() + "'"; 
    }

    std::string sql = "UPDATE files \
        SET url = " + url + " , " +
        "parentId = " + parentId + ", " +
        "type = '" + item_type_to_str.at(update_item.type) + "', "
        "size = " + size + " " +
        "WHERE id = '" + update_item.id + "'";

    tx.exec(sql);
    tx.commit();
}

void db_adapter::UpdateItem(const std::vector<item_imports>& update_items) {
    for(const auto& item : update_items) {
        UpdateItem(item);
    }
}

void db_adapter::DeleteItem(const std::string& id) {
    pqxx::work tx(*db_con_);

    std::string sql = "DELETE FROM files \
        WHERE id = '" + id + "'" ;

    tx.exec(sql);
    tx.commit();
}