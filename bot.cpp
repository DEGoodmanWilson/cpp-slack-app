//
// Copyright © 2015–2016 D.E. Goodman-Wilson All rights reserved.
//

#include "bot.h"
#include <iostream>

bot::bot(const slack::user &bot_user, const slack::team_id &team, const std::string &token) :
        bot_user_{bot_user}, team_{team}, token_{token}
{
    client_ = std::make_unique<slack::real_time_client>(
            slack::real_time_client::parameter::auto_ping{true},
            slack::real_time_client::parameter::auto_reconnect{true});

    client_->register_event_handler<slack::event::message>(std::bind(&bot::handle_event_, this, std::placeholders::_1));

    bot_thread_ = std::thread{
            [this]() {
                this->client_->start();
            }
    };
}

bot::~bot()
{
    //TODO destroy child thread, then client_;
}

void bot::on(std::vector<bot_event> events, std::function<void(const message &message)> callback)
{
    for (auto &event :events)
    {
        on_callbacks_[event].emplace_back(callback);
    }
}

void bot::hears(std::regex regex, std::vector<bot_event> events, std::function<void(const message &message)> callback)
{
    for (auto &event :events)
    {
        hears_callbacks_[event].emplace_back(std::make_pair(regex, callback));
    }
}

void bot::say(const slack::channel_id &channel, const std::string &message)
{
    auto m = slack::event::message{};
    m.channel = channel;
    m.text = message;
    //TODO client_->send_message
}

void bot::reply(const message &message, const std::string &response)
{
    auto m = slack::event::message{};
    m.channel = message.channel;
    m.text = response;
    //TODO client_->send_message(make a message from the response
}


void bot::handle_message_(const message &message)
{
//first, what _kind_ of message is this?
    bot_event ev{bot_event::any_message};

//check if @name is at beginning, or somewhere else, or the message is a DM.
    auto loc = message.text.find_first_of("@" + bot_user_.name);
    if (loc != std::string::npos) //found it!
    {
        if (loc == 0)
            ev = bot_event::direct_mention;
        else
            ev = bot_event::mention;
    }
    else if ((message.channel)[0] == 'D') //direct message? TODO
    {
        ev = bot_event::direct_message;
    }

//check for event handlers
    auto callbacks = on_callbacks_[ev];
    for (auto &cb : callbacks)
    {
        cb(message);
    }

//check for hears callbacks
    auto hears_callbacks = hears_callbacks_[ev]; //this is a map of regexes to vector of callbacks
    for (auto &cb : hears_callbacks)
    {
        auto regex = cb.first;
        auto callback = cb.second;
        if (std::regex_match(message.text, regex))
        {
            //we have a match
            callback(message);
        }
    }
}

void bot::handle_event_(std::shared_ptr<slack::event::message> raw_message)
{
    std::cout << "EVENT! " << raw_message->event_name << " " << *raw_message->text << std::endl;
    message processed_message{*raw_message->user, *raw_message->channel, *raw_message->ts, *raw_message->text};

    handle_message_(processed_message);
}
