//
// Copyright © 2015 Slack Technologies, Inc. All rights reserved.
//

#include "app_web_controller.h"
#include <slack/oauth.access.h>

void app_web_controller::get_app_page(Mongoose::Request &request, Mongoose::StreamResponse &response)
{
    response <<
    "<html><h1>Add the Demo App to your team!</h1><a href=\"https://slack.com/oauth/authorize?scope=incoming-webhook,commands&client_id=" +
    client_id_ +
    "\"><img alt=\"Add to Slack\" height=\"40\" width=\"139\" src=\"https://platform.slack-edge.com/img/add_to_slack.png\" srcset=\"https://platform.slack-edge.com/img/add_to_slack.png 1x, https://platform.slack-edge.com/img/add_to_slack@2x.png 2x\"></a></html>" <<
    endl;
}

void app_web_controller::oauth(Mongoose::Request &request, Mongoose::StreamResponse &response)
{
    auto code = request.get("code");

    if (code.empty()) //error condition!
    {
        response << "Hi there!" << std::endl; //I'd like to do a 404 or something here
        return;
    }

    auto slack_response = slack::oauth::access(client_id_, client_secret_, code, slack::oauth::parameter::access::redirect_uri{redirect_uri_});

    if(slack_response)
    {
        std::cout << slack_response.raw_json << std::endl;
        //store the access token!
        db_->store_token(*slack_response.access_token);

        response << "<h1>Success!</h1>" << std::endl;
    }
    else //error!
    {
        response << "<h1>ERROR!</h1>" << slack_response.error.value() << std::endl;
    }
}

void app_web_controller::command(Mongoose::Request &request, Mongoose::StreamResponse &response)
{
    slack::command cmd{request.getAllVariable()};

    std::cout << "Got a command! " << cmd.command_name;


    //TODO spin this off into a thread!! also, do something cool.
    response << "Well howdy, " + cmd.user_name << std::endl;
    return;


}

