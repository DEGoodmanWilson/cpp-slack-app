//
// Copyright © 2015 Slack Technologies, Inc. All rights reserved.
//

#pragma once

#include <mongoose/WebController.h>

class app_web_controller : public Mongoose::WebController
{
public:
    app_web_controller(const string &client_id_, const string &client_secret_,
                       const string &redirect_uri_) : client_id_(client_id_), client_secret_(client_secret_), redirect_uri_(redirect_uri_)
    { }

    void get_app_page(Mongoose::Request &request, Mongoose::StreamResponse &response);
    void oauth(Mongoose::Request &request, Mongoose::StreamResponse &response);


    void hello(Mongoose::Request &request, Mongoose::StreamResponse &response);

    void setup()
    {
        registerRoute("GET", "/", new Mongoose::RequestHandler<app_web_controller, Mongoose::StreamResponse>(this, &app_web_controller::get_app_page ));
        registerRoute("GET", "/oauth", new Mongoose::RequestHandler<app_web_controller, Mongoose::StreamResponse>(this, &app_web_controller::oauth ));
        registerRoute("GET", "/hello", new Mongoose::RequestHandler<app_web_controller, Mongoose::StreamResponse>(this, &app_web_controller::hello ));
    }

private:
    std::string client_id_, client_secret_, redirect_uri_;
};