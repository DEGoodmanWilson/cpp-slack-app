//
// Copyright © 2015 Slack Technologies, Inc. All rights reserved.
//

#pragma once

#include <mongoose/WebController.h>

class app_web_controller : public Mongoose::WebController
{
public:
    void get_app_page(Mongoose::Request &request, Mongoose::StreamResponse &response);

    void hello(Mongoose::Request &request, Mongoose::StreamResponse &response);

    void setup()
    {
        registerRoute("GET", "/", new Mongoose::RequestHandler<app_web_controller, Mongoose::StreamResponse>(this, &app_web_controller::get_app_page ));
        registerRoute("GET", "/hello", new Mongoose::RequestHandler<app_web_controller, Mongoose::StreamResponse>(this, &app_web_controller::hello ));
    }
};