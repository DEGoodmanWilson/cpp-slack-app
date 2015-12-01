#include <iostream>
#include <slack/slack.h>
#include <cpr.h>
#include <mongoose/Server.h>
#include "app_web_controller.h"

int main(int argc, char **argv)
{
    //pass in the app client ID at the command line
    if(argc < 4)
    {
        std::cout << "Usage: "+std::string{argv[0]}+" client_id client_secret code [redirect_uri]" << std::endl;
        exit(-1);
    }

    auto slack_client_id = std::string{argv[1]};
    auto slack_client_secret = std::string{argv[2]};
    std::string slack_redirect_uri{""};
    if(argc > 3) slack_redirect_uri = std::string{argv[3]};

    //zeroth, let's set up libslack
    slack::http::get = [](std::string url, slack::http::params params) -> slack::http::response {
        cpr::Parameters p;
        for (auto &kv : params)
        {
            p.AddParameter({kv.first, kv.second});
        }

        auto response = cpr::Get(cpr::Url{url}, p);

        return {static_cast<uint32_t>(response.status_code), response.text};
    };

    slack::http::post = [](std::string url, slack::http::params params) -> slack::http::response {
        cpr::Parameters p;
        for (auto &kv : params)
        {
            p.AddParameter({kv.first, kv.second});
        }

        auto response = cpr::Post(cpr::Url{url}, p);

        return {static_cast<uint32_t>(response.status_code), response.text};
    };

    //First, let's set up some persistent storage. sqlite!

    //Second, fire up a webserver for handling oauth requests
    app_web_controller my_controller{slack_client_id, slack_client_secret, slack_redirect_uri};
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