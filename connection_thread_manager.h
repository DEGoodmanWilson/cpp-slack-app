//
// Copyright © 2015–2016 D.E. Goodman-Wilson All rights reserved.
//

//TODO do I still need this? Extraneous!

#pragma once

#include <map>
#include <string>
#include <thread>

class connection_thread_manager {
public:
    connection_thread_manager() : running_{true} {};
    virtual ~connection_thread_manager();

    void launch_thread(const std::string& team_id, const std::string& token);
private:
    std::atomic<bool> running_;
    std::map<std::string, std::thread> threads_;
};
