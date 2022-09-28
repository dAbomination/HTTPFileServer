#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace common {
    
    enum item_type {
        FILE_ITEM = 1,
        FOLDER_ITEM = 2
    };

    static const std::unordered_map<common::item_type, std::string> item_type_to_str = {
        { common::item_type::FILE_ITEM,     "FILE"      },
        { common::item_type::FOLDER_ITEM,   "FOLDER"    }
    };

static const std::unordered_map<std::string, common::item_type> str_to_item_type = {
        { "FILE",       common::item_type::FILE_ITEM    },
        { "FOLDER",     common::item_type::FOLDER_ITEM  }
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
