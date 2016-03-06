//
// Copyright © 2015–2016 D.E. Goodman-Wilson All rights reserved.
//

#include "app_web_controller.h"
#include <slack/slack.h>
#include <json/json.h>
#include <cpr.h>
#include <future>

const std::string host{"slack.com"};


app_web_controller::app_web_controller(std::shared_ptr<app_persistent_store> db,
                                       const string &client_id_,
                                       const string &client_secret_,
                                       const string &redirect_uri_) :
        db_{db}, client_id_(client_id_), client_secret_(client_secret_), redirect_uri_(redirect_uri_)
{
    slack::set_host(host);
}

void app_web_controller::get_app_page(Mongoose::Request &request, Mongoose::StreamResponse &response)
{
    response <<
    "<html><h1>Add the Demo App to your team!</h1><a href=\"https://"+host+"/oauth/authorize?scope=incoming-webhook,commands&client_id=" +
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

    auto slack_response = slack::oauth::access(client_id_,
                                               client_secret_,
                                               code,
                                               slack::oauth::access::parameter::redirect_uri{redirect_uri_});

    if (slack_response)
    {
        std::cout << slack_response.raw_json << std::endl;
        //store the access token!
        db_->store_token(*slack_response.team_id, *slack_response.access_token);

        response << "<h1>Success!</h1>" << std::endl;
    }
    else //error!
    {
        response << "<h1>ERROR!</h1>" << *slack_response.error_message << std::endl;
    }
}

void app_web_controller::command(Mongoose::Request &request, Mongoose::StreamResponse &response)
{
    slack::command cmd{request.getAllVariable()};

    //we capture f to avoid a situation where this would be sync
    auto f = std::async(std::launch::async, [=] {
        //construct the query and URL
        std::string url = "http://en.wikipedia.org/w/api.php";
        //?action=opensearch&search=searchtext&format=json&limit=4"
        cpr::Parameters params = {
                {"action", "opensearch"},
                {"search", cmd.text},
                {"format", "json"},
                {"limit",  "4"},
        };

        auto search_result = cpr::Get(cpr::Url{url}, params);

        Json::Value wiki_array;
        Json::Reader reader;
        bool parsed_success = reader.parse(search_result.text, wiki_array, false);
        if (!parsed_success)
        {
            auto payload = slack::incoming_webhook::payload{
                    slack::incoming_webhook::payload::parameter::text{"Error contacting Wikipedia"}
            };
            cpr::Payload params{
                    {"payload", payload},
            };

            auto result = cpr::Post(cpr::Url{cmd.response_url}, params);
        }
        else
        {
            std::string message_text;


            message_text += "<@" + cmd.user_id + "|" + cmd.user_name + "> searched for *" + cmd.text + "*.\n";


            bool disambiguation_check{false};
            if (wiki_array[2][0].asString().find("may refer to:") != std::string::npos)
            {
                disambiguation_check = true;
            }

            auto message_primary_title = wiki_array[1][0].asString();
            auto message_primary_summary = wiki_array[2][0].asString();
            auto message_primary_link = wiki_array[3][0].asString();
            std::string message_other_options_title;
            std::string message_attachment_text;
            std::string message_other_options;

            if (wiki_array[1].size() == 0)
            {
                message_attachment_text = "Sorry! I couldn't find anything like that.";
            }
            else
            {
                if (disambiguation_check)
                {
                    message_text += "There are several possible results for ";
                    message_text += "*<" + message_primary_link + "|" + cmd.text + ">*+\n";
                    message_text += message_primary_link;
                    message_other_options_title = "Here are some of the possibilities:";
                }
                else
                {
                    message_text += "*<" + message_primary_link + "|" + message_primary_title + ">*\n";
                    message_text += message_primary_summary + "\n";
                    message_text += message_primary_link;
                    message_other_options_title = "Here are a few other options:";
                }

                auto other_options = wiki_array[3];
                for (auto value : other_options)
                {
                    message_other_options += value.asString() + "\n";
                }
            }

            //now build the data structures
            auto payload = slack::incoming_webhook::payload{
                    message_text,
                    slack::incoming_webhook::payload::parameter::mrkdwn{true},
                    slack::incoming_webhook::payload::parameter::attachments{
                            slack::attachment::parameter::color{"#b0c4de"},
                            slack::attachment::parameter::fallback{message_attachment_text},
                            slack::attachment::parameter::text{message_attachment_text},
                            slack::attachment::parameter::mrkdwn_in{
                                    slack::attachment::parameter::mrkdwn_in_fields::fallback,
                                    slack::attachment::parameter::mrkdwn_in_fields::text
                            },
                            slack::attachment::parameter::fields{
                                    slack::field{message_other_options_title, message_other_options}
                            }
                    }
            };

            cpr::Payload params{
                    {"payload", payload},
            };

            auto result = cpr::Post(cpr::Url{cmd.response_url}, params);
            std::cout << result.text << std::endl;
        }


    });

    response << std::endl;
}

