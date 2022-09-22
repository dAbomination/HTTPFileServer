#pragma once

#include <pqxx/pqxx>
#include <string>
#include <optional>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string_view>

#include "common.h"

static const std::unordered_map<common::item_type, std::string> item_type_to_str = {
    { common::item_type::FILE_ITEM,     "FILE"      },
    { common::item_type::FOLDER_ITEM,   "FOLDER"    }
};

static const std::unordered_map<std::string, common::item_type> str_to_item_type = {
    { "FILE",       common::item_type::FILE_ITEM    },
    { "FOLDER",     common::item_type::FOLDER_ITEM  }
};

// Implements adding, deleting and updating the database
class db_adapter {
public:
    db_adapter();

    // Add all dependencies to paths
    void GetIds(common::FileDependencies& paths);

    common::item_imports GetItemInfo(const std::string& id);
    // Get ids that have been updated in period [date, date-24h] 
    std::unordered_set<std::string> GetUpdatedIds(
        const std::string& from_date,
        const std::string& to_date);
    // Get history of item from [date_start, date_end)
    std::vector<common::update_date_data> GetItemHistory(
        const std::string& id,
        const std::string& date_start,
        const std::string& date_end);

    // Insert single item or vector
    void InsertItem(const common::item_imports& import_item);
    void InsertItem(const std::vector<common::item_imports>& import_items);
    // Update signle item or vector
    void UpdateItem(const common::item_imports& update_item);
    void UpdateItem(const std::vector<common::item_imports>& update_items);
    // Insert information about updates (time, id and raw json data)
    void InsertUpdates(const common::update_date_data& data);
    void InsertUpdates(const std::vector<common::update_date_data>& data);

    void DeleteItem(const std::string& id);
    void DeleteUpdates(const std::string& id);
private:
    std::unique_ptr<pqxx::connection> db_con_;    
};