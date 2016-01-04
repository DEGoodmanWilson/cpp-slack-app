//
// Created by Donald Goodman-Wilson on 1/4/16.
//

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
