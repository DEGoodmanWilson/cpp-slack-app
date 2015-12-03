//
// Copyright © 2015 Slack Technologies, Inc. All rights reserved.
//

#pragma once

#include <slack/types.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <vector>
#include <tuple>

class app_persistent_store
{
public:
    app_persistent_store(const std::string &db_filename);
    void store_token(const slack::team_id& team_id, const std::string& token);
    std::vector<std::tuple<slack::team_id, std::string>> get_all_tokens(void);
private:
    SQLite::Database db_;
};


