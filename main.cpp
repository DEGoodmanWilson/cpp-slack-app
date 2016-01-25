//
// Copyright © 2015–2016 D.E. Goodman-Wilson All rights reserved.
//

#include <iostream>
//#include <slack/slack.h>
#include <mongoose/Server.h>
#include "app_persistent_store.h"
#include "app_web_controller.h"
//#include "connection_thread_manager.h"
#include <memory>

int main(int argc, char **argv)
{
    //pass in the app client ID at the command line
    if (argc < 4)
    {
        std::cout << "Usage: " + std::string{argv[0]} + " client_id client_secret [redirect_uri]" << std::endl;
        exit(-1);
    }

    auto slack_client_id = std::string{argv[1]};
    auto slack_client_secret = std::string{argv[2]};
    std::string slack_redirect_uri{""};
    if (argc > 3) slack_redirect_uri = std::string{argv[3]};

    //First, let's set up some persistent storage. sqlite! TODO you'll want to set this filename yourself
    auto db = std::make_shared<app_persistent_store>("/Users/dgoodman/Documents/Projects/redstone/tokens.db");


    //Second, fire up a webserver for handling oauth requests
    app_web_controller my_controller{db, slack_client_id, slack_client_secret, slack_redirect_uri};
    Mongoose::Server server(8080);
    server.registerController(&my_controller);
    server.start();

    //Now, let's fire up all the app instances we are current serving.
//    auto thread_manager = std::make_shared<connection_thread_manager>();
    // NOTE! This is not really the best way to do this—these should be forked to run in their own proceess,
    //  and then monitored to see if they crash.
//    for(const auto& token : db->get_all_tokens())
//    {
//        std::cout << std::get<0>(token) << " " << std::get<1>(token) << std::endl;
//        thread_manager->launch_thread(std::get<0>(token), std::get<1>(token));
//    }


    //and then…we just sit here waiting to be killed.
    //TODO would be better to watch a CV? Or, at least do something productive with this thread? Perhaps use this as
    // a place to post events? That sounds pretty alright.
    while (1)
    {
        sleep(10);
    }
}