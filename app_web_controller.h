//
// Copyright © 2015–2016 D.E. Goodman-Wilson All rights reserved.
//

#pragma once

#include "app_persistent_store.h"
#include <mongoose/WebController.h>
#include <memory>

class app_web_controller :
        public Mongoose::WebController
{
public:
    app_web_controller(std::shared_ptr<app_persistent_store> db,
                       const string &client_id_,
                       const string &client_secret_,
                       const string &redirect_uri_) :
            db_{db}, client_id_(client_id_), client_secret_(client_secret_), redirect_uri_(redirect_uri_)
    { }

    void get_app_page(Mongoose::Request &request, Mongoose::StreamResponse &response);
    void oauth(Mongoose::Request &request, Mongoose::StreamResponse &response);
    void command(Mongoose::Request &request, Mongoose::StreamResponse &response);


    void setup()
    {
        registerRoute("GET", "/", new Mongoose::RequestHandler<app_web_controller, Mongoose::StreamResponse>(this, &app_web_controller::get_app_page));
        registerRoute("GET", "/oauth", new Mongoose::RequestHandler<app_web_controller, Mongoose::StreamResponse>(this, &app_web_controller::oauth));
        registerRoute("POST", "/command", new Mongoose::RequestHandler<app_web_controller, Mongoose::StreamResponse>(this, &app_web_controller::command));
    }

private:
    std::shared_ptr<app_persistent_store> db_;
    std::string client_id_, client_secret_, redirect_uri_;
};