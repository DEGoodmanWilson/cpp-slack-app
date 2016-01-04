//
// Created by Donald Goodman-Wilson on 1/4/16.
//

#include "connection_thread_manager.h"


connection_thread_manager::~connection_thread_manager()
{
    //TODO halt and join all threads
    running_ = false;

    for(auto& t: threads_)
    {
       auto& thread = std::get<1>(t);
       if(thread.joinable()) thread.join();
    }
}

void connection_thread_manager::launch_thread(const std::string& team_id, const std::string &token)
{
    if(threads_.count(team_id)) return; //this key already exists, do nothing

    threads_[team_id] = std::thread{[this, team_id, token](){
        while(this->running_)
        {
            //TODO this is where we connect, &c. What we really want here is a more nuanced class than a simple lambda
        }
    }};
}
