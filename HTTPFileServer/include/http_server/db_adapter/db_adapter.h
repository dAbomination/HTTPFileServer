#pragma once

#include <pqxx/pqxx>
#include <string>
#include <optional>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string_view>

enum item_type {
    FILE_ITEM = 1,
    FOLDER_ITEM = 2
};

struct item_imports{
    std::string id;
    std::optional<std::string> url = std::nullopt;
    std::optional<std::string> parentId = std::nullopt;
    item_type type;
    std::optional<int64_t> size = std::nullopt;
    std::string updateDate;
};

static const std::unordered_map<item_type, std::string> item_type_to_str = {
    { item_type::FILE_ITEM,     "FILE"      },
    { item_type::FOLDER_ITEM,   "FOLDER"    }
};

static const std::unordered_map<std::string, item_type> str_to_item_type = {
    { "FILE",       item_type::FILE_ITEM    },
    { "FOLDER",     item_type::FOLDER_ITEM  }
};

// [item to id, its children] 
using FileDependencies = std::unordered_map<std::string, std::vector<std::string>>;

// Implements adding, deleting and updating the database
class db_adapter {
public:
    db_adapter();

    // Add all dependencies to paths
    void GetIds(FileDependencies& paths);

    item_imports GetItemInfo(const std::string& id);
    // Insert single item or vector
    void InsertItem(const item_imports& import_item);
    void InsertItem(const std::vector<item_imports>& import_items);
    // Update signle item or vector
    void UpdateItem(const item_imports& update_item);
    void UpdateItem(const std::vector<item_imports>& update_items);

    void DeleteItem(const std::string& id);
private:
    std::unique_ptr<pqxx::connection> db_con_;    
};