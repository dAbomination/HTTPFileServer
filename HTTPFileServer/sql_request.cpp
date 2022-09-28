#include "sql_request.h"

namespace sql_request {

    std::string Insert(const std::string& db_name, const common::item_imports& data) {
        std::string parentId = "null";
        if(data.parentId.has_value()) {
            parentId = "'" + data.parentId.value() + "'";
        }    
        std::string size = "null";
        if(data.size.has_value()) {
            size = std::to_string(data.size.value()); 
        }
        std::string url = "null";
        if(data.url.has_value()) {
            url = "'" + data.url.value() + "'"; 
        }

        std::string sql = "INSERT INTO " + db_name + " \
            VALUES('" + data.id + "', " +
            url + " , " + 
            parentId + ", '" +
            common::item_type_to_str.at(data.type) + "'," +
            size + " , '" +
            data.updateDate + "')";        
        
        return sql;
    }

    std::string Insert(const std::string& db_name, const common::update_date_data& data) {
        std::string sql = "INSERT INTO " + db_name + " \
            VALUES('" + data.updateDate + "', '" +
            data.id + "' , '" + 
            data.raw_json_data + "')";
        
        return sql;
    }

} // namespace sql_request