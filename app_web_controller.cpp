//
// Copyright © 2015 Slack Technologies, Inc. All rights reserved.
//

#include "app_web_controller.h"
#include <slack/oauth.access.h>
#include <future>
#include <cpr.h>
#include <json/json.h>

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

    auto slack_response = slack::oauth::access(client_id_,
                                               client_secret_,
                                               code,
                                               slack::oauth::parameter::access::redirect_uri{redirect_uri_});

    if (slack_response)
    {
        std::cout << slack_response.raw_json << std::endl;
        //store the access token!
        db_->store_token(*slack_response.team_id, *slack_response.access_token);

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

        std::string response;
        Json::Value wiki_array;
        Json::Reader reader;
        bool parsed_success = reader.parse(search_result.text, wiki_array, false);
        if (!parsed_success)
        {
            response = "Error contacting Wikipedia";
            //TODO DRY!
            auto payload = slack::incoming_webhook::payload::create_payload(response);
            cpr::Payload params{
                    {"payload", payload},
            };

            auto result = cpr::Post(cpr::Url{cmd.response_url}, params);
        }
        else
        {
            slack::attachment attachment;
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

            attachment.color = {"#b0c4de"};
            attachment.fallback = message_attachment_text;
            attachment.text = message_attachment_text;
            attachment.mrkdwn_in = std::vector<std::string>{"fallback", "text"};
            slack::field field;
            field.title = message_other_options_title;
            field.value = message_other_options;
            attachment.fields = std::vector<slack::field>{field};

            auto payload = slack::incoming_webhook::payload::create_payload(message_text,
                                                                            slack::incoming_webhook::parameter::mrkdwn{
                                                                                    true},
                                                                            slack::incoming_webhook::parameter::attachments{attachment});

            cpr::Payload params{
                    {"payload", payload},
            };

            auto result = cpr::Post(cpr::Url{cmd.response_url}, params);
            std::cout << result.text << std::endl;
        }


    });

    response << std::endl;
}

