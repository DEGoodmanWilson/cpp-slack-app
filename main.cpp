#include <iostream>
#include <slack/slack.h>
#include <mongoose/Server.h>
#include "app_web_controller.h"

std::string SLACK_CLIENT_ID;

int main(int argc, char **argv)
{
    //pass in the app client ID at the command line
    if(argc < 2)
    {
        exit(-1);
    }
    SLACK_CLIENT_ID = {argv[1]};

    //First, let's set up some persistent storage. sqlite!

    //Second, fire up a webserver for handling oauth requests
    app_web_controller my_controller;
    Mongoose::Server server(8080);
    server.registerController(&my_controller);
    server.start();

    //Now, let's fire up all the app instances we are current serving.
    // NOTE! This is not really the best way to do this—these should be forked to run in their own proceess,
    //  and then monitored to see if they crash.


    //and then…we just sit here waiting to be killed.
    while (1) {
        sleep(10);
    }
}