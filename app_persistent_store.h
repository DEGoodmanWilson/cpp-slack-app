//
// Copyright © 2015 Slack Technologies, Inc. All rights reserved.
//

#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <vector>

class app_persistent_store
{
public:
    app_persistent_store(const std::string &db_filename);
    void store_token(const std::string& token);
    bool validate_token(const std::string& token);
    std::vector<std::string> get_all_tokens(void);
private:
    SQLite::Database db_;
};


