//
// Copyright © 2015–2016 D.E. Goodman-Wilson All rights reserved.
//

#include "app_persistent_store.h"

app_persistent_store::app_persistent_store(const std::string &db_filename)
        : db_{db_filename, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE}
{
    if (!db_.tableExists("tokens"))
    {
        //make the tokens table
        db_.exec("CREATE TABLE tokens (team_id TEXT PRIMARY KEY, token TEXT)");
        db_.exec("CREATE INDEX token_idx ON tokens(token)");
    }
}

void app_persistent_store::store_token(const slack::team_id &team_id, const std::string &token)
{
    //TODO obviously, you'll want to sanitize these inserts!
    db_.exec("INSERT INTO tokens VALUES ('"+team_id+"', '" + token + "')");
}


std::vector<std::tuple<slack::team_id, std::string>> app_persistent_store::get_all_tokens(void)
{
    SQLite::Statement query{db_, "SELECT team_id, token FROM tokens"};
    std::vector<std::tuple<slack::team_id, std::string>> retval;
    while (query.executeStep())
    {
        retval.emplace_back(std::make_tuple(query.getColumn(0).getText(), query.getColumn(1).getText()));
    }

    return retval;
}
