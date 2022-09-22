#pragma once

namespace common {
    
    enum item_type {
        FILE_ITEM = 1,
        FOLDER_ITEM = 2
    };

    // using for inserting into "filesdata" table
    struct item_imports{
        std::string id;
        std::optional<std::string> url = std::nullopt;
        std::optional<std::string> parentId = std::nullopt;
        item_type type;
        std::optional<int64_t> size = std::nullopt;
        std::string updateDate;
    };
    // using for inserting into "updates" table
    struct update_date_data{
        std::string updateDate;
        std::string id;
        std::string raw_json_data;
    };

    // [item to id, its children] 
    using FileDependencies = std::unordered_map<std::string, std::unordered_set<std::string>>;
} //namespace common
