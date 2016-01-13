//
// Copyright © 2015–2016 D.E. Goodman-Wilson All rights reserved.
//

#pragma once

#include <slack/slack.h>
#include <vector>
#include <string>
#include <functional>
#include <regex>
#include <map>
#include <memory>

enum class bot_event
{
    any_message     = 0,
    direct_mention  = 1,
    mention         = 2,
    direct_message  = 3,
};

struct message //TODO what's wrong with a Slack message?
{
    slack::user_id user;
    slack::channel_id channel;
    slack::ts ts;
    std::string text;
};

//loosely modeled after botkit
/**
 * Anticipated usage would be something like:
 * bot b{"T1234", "xoxb-1234"};
 * if(!b) freak_out(); //an error
 * b.on({bot_event::direct_mention}, [](auto message){
 *   auto user = slack::users::info(message.user); //too easy. should cache this obviously
 *   b.reply(message, "howdy " + user.first_name + "!");
 * });
 * b.hears(/what time is it/, {bot_event::any_message}, [](auto message){
 *   b.reply(message, "four-thirty");
 * });
 */
class bot
{
public:
    bot(const slack::user& bot_user, const slack::team_id& team, const std::string& token);
    ~bot();

    //callbacks for listening
    //TODO this would be so much nicer if we could de-register these. Can we not use NOD here? Or something similar?
    void on(std::vector<bot_event> events, std::function<void(const message& message)> callback);
    void hears(std::regex regex, std::vector<bot_event> events, std::function<void(const message& message)> callback);

    //actions for acting
    void say(const slack::channel_id& channel, const std::string& message);
    void reply(const message& message, const std::string& response);

private:
    void handle_message_(const message& message);
    void handle_event_(std::shared_ptr<slack::event::message>);
    //TODO others.

    slack::user bot_user_;
    slack::team_id team_;
    std::string token_;
    std::map<bot_event, std::vector<std::function<void(const message&)>>> on_callbacks_;
    std::map<bot_event, std::vector<std::pair<std::regex, std::function<void(const message&)>>>> hears_callbacks_; //TODO this is almost certianly the wrong data structure here
    std::unique_ptr<slack::real_time_client> client_; //TODO does this need to be a pointer? Really?
    std::thread bot_thread_;
};