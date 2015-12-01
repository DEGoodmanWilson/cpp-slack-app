//
// Copyright © 2015 Slack Technologies, Inc. All rights reserved.
//

#include "app_web_controller.h"

extern std::string SLACK_CLIENT_ID;

void app_web_controller::get_app_page(Mongoose::Request &request, Mongoose::StreamResponse &response)
{
    response << "<html><h1>Add the Demo App to your team!</h1><a href=\"https://slack.com/oauth/authorize?scope=incoming-webhook,commands&client_id="+SLACK_CLIENT_ID+"\"><img alt=\"Add to Slack\" height=\"40\" width=\"139\" src=\"https://platform.slack-edge.com/img/add_to_slack.png\" srcset=\"https://platform.slack-edge.com/img/add_to_slack.png 1x, https://platform.slack-edge.com/img/add_to_slack@2x.png 2x\"></a></html>" << endl;
}


void app_web_controller::hello(Mongoose::Request &request, Mongoose::StreamResponse &response)
{
    response << "Hello " << htmlEntities(request.get("name", "... what's your name ?")) << endl;
}
