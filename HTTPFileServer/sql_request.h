#pragma once

#include <string>
#include <vector>

#include "common.h"

namespace sql_request {

    std::string Insert(const std::string& db_name, const common::item_imports& data);
    std::string Insert(const std::string& db_name, const common::update_date_data& data);

} // namespace sql_request