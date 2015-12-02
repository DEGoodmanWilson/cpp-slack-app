//
// Copyright © 2015 Slack Technologies, Inc. All rights reserved.
//

#include "app_persistent_store.h"

app_persistent_store::app_persistent_store(const std::string &db_filename)
        : db_{db_filename, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE}
{
    if (!db_.tableExists("tokens"))
    {
        //make the tokens table
        db_.exec("CREATE TABLE tokens (id INTEGER PRIMARY KEY, token TEXT)");
        db_.exec("CREATE INDEX token_idx ON tokens(token)");
    }
}

void app_persistent_store::store_token(const std::string &token)
{
    //TODO obviously, you'll want to sanitize these inserts!
    db_.exec("INSERT INTO tokens VALUES (NULL, \""+token+"\")");
}


bool app_persistent_store::validate_token(const std::string &token) {
    SQLite::Statement query{db_, "SELECT token FROM tokens WHERE token=\""+token+"\""};
    bool retval = false;
    while(query.executeStep())
    {
        retval = true; //because we got at least one row! There has to be a better way to do this
    }

    return retval;
}

std::vector<std::string> app_persistent_store::get_all_tokens(void)
{
    SQLite::Statement query{db_, "SELECT token FROM tokens"};
    std::vector<std::string> retval;
    while (query.executeStep())
    {
        retval.emplace_back(query.getColumn(0).getText());
    }

    return retval;
}
