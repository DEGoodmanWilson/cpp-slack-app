//
// Copyright © 2015 Slack Technologies, Inc. All rights reserved.
//

#pragma once

#include <mongoose/WebController.h>

class app_web_controller : public Mongoose::WebController
{
public:
    void hello(Mongoose::Request &request, Mongoose::StreamResponse &response)
    {
        response << "Hello " << htmlEntities(request.get("name", "... what's your name ?")) << endl;
    }

    void setup()
    {
        addRoute("GET", "/hello", app_web_server, hello);
    }
};