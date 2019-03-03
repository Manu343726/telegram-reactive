#include <csignal>
#include <cstdio>
#include <string>
#include <tgbot/tgbot.h>
#include <fmt/format.h>
#include <unordered_map>
#include <sstream>
#include <range/v3/view.hpp>
#include <range/v3/action.hpp>
#include <usingstdcpp2019/urp.hpp>

std::string getUserFullName(const TgBot::User::Ptr user)
{
    if(not user)
    {
        return "unknown";
    }

    std::string fullName = user->firstName;

    if(not user->lastName.empty())
    {
        fullName += " " + user->lastName;
    }

    if(not user->username.empty())
    {
        fullName += fmt::format(" (@{})", user->username);
    }

    return fullName;
}

using TelegramTrigger = usingstdcpp2019::urp::trigger<TgBot::Message::Ptr>;

class ReactiveBot : public TgBot::Bot, public TelegramTrigger
{
public:
    ReactiveBot(std::string apiKey) :
        TgBot::Bot{std::move(apiKey)}
    {
        // Magic happens here
        TgBot::Bot::getEvents().onAnyMessage([this](TgBot::Message::Ptr message)
        {
            TelegramTrigger::operator=(message);
        });
    }
};

const auto anyCommand = usingstdcpp2019::urp::filter([](TgBot::Message::Ptr message)
{
    return not message->text.empty() and message->text.front() == '/';
});

const auto textMessages = usingstdcpp2019::urp::filter([](TgBot::Message::Ptr message)
{
    return message->text.empty() or message->text.front() != '/';
});


constexpr auto command = [](const std::string& name)
{
    return usingstdcpp2019::urp::filter([name](TgBot::Message::Ptr message)
    {
        return not message->text.empty() and message->text.front() == '/' and
            message->text.substr(1) == name;
    });
};

constexpr auto fromUser = [](const std::string& name)
{
    return usingstdcpp2019::urp::filter([name](TgBot::Message::Ptr message)
    {
        return message->from and getUserFullName(message->from) == name;
    });
};

int main() {
    std::string token(std::getenv("USINGSTDCPP2019_BOT_TOKEN"));
    std::unordered_map<std::int32_t, TgBot::User::Ptr> knownUserIds;

    // ReactiveBot is both a Telegram bot and a urp trigger
    // Events are raised whenever a message arrives to the bot
    ReactiveBot bot(token);

    // Subscript to any incoming message (Text, commands, whatever)
    bot.connect([&](const auto&, TgBot::Message::Ptr message)
    {
        if(message->from)
        {
            if(not knownUserIds.count(message->from->id))
            {
                knownUserIds[message->from->id] = message->from;

                fmt::print("Look, a new user sent me a message. {}, welcome to usingstdcpp 2019!\n",
                    getUserFullName(message->from));
            }
        }
    });

    // Subscript to text messages from users
    auto messages = bot | textMessages;
    messages.connect([&](const auto&, TgBot::Message::Ptr message)
    {
        if(message->from and knownUserIds.count(message->from->id))
        {
            fmt::print("@{} says \"{}\"\n", getUserFullName(message->from), message->text);

            bot.getApi().sendMessage(message->chat->id,
                "You know I'm a poor bot coded in one afternoon right?\n");
            bot.getApi().sendMessage(message->chat->id,
                "I'm afraid I will not be able to respond you with anything intelligible. Sorry\n");
        }
    });

    // Handle /peers command requests
    auto peersCommand = bot | command("peers");
    peersCommand.connect([&](const auto&, TgBot::Message::Ptr message) {
        if(knownUserIds.empty() || not message->from || not knownUserIds.count(message->from->id))
        {
            bot.getApi().sendMessage(message->chat->id, "You're not allowed to ask that!");
            return;
        }

        const auto users = knownUserIds
                         | ranges::view::values
                         | ranges::view::transform([](TgBot::User::Ptr user){ return user->username; })
                         | ranges::view::transform([](const std::string& s) { return fmt::format("@{}\n", s); })
                         | ranges::action::join
                         | ranges::to_<std::string>();

        if(message->from)
        {
            fmt::format("{} asked for the full user list:\n{}\n",
                getUserFullName(message->from), users);
        }

        bot.getApi().sendMessage(message->chat->id,
            "I'm glad you asked! This is the list of users that already talked with me:\n");
        bot.getApi().sendMessage(message->chat->id, users);
    });

    // Handle /help command requests
    auto helpCommand = bot | command("help");
    helpCommand.connect([&](const auto&, TgBot::Message::Ptr message)
    {
        bot.getApi().sendMessage(message->chat->id,
            "Need help? I know I'm supposed to tell you the set of my available commands, "
            "but I'm not even sure myself. Try with /peers. Good luck");
    });

    // Intercept any command sent by Manu and tell him who is the boss here
    auto manuCommands = bot | anyCommand | fromUser("Manu343726");
    manuCommands.connect([&](const auto&, TgBot::Message::Ptr message)
    {
        bot.getApi().sendMessage(message->chat->id,
            "Sorry Manu, but I tould you you're not allowed to ask me anything...");

        fmt::print("Here's Manu trolling me again with weird commands... \"{}\" ? What is that supposed to mean?\n",
            message->text.substr(1));
    });

    std::signal(SIGINT, [](int s) {
        std::printf("Interrupt, exiting...\n");
        std::exit(EXIT_SUCCESS);
    });

    try {
        fmt::print("Hi I'm @{}, I'm ready to talk with you!\n\n", bot.getApi().getMe()->username);
        bot.getApi().deleteWebhook();

        // Telegram API works through HTTP long polling, so polling requests
        // should be sent again whenever the server responds to get "real-time" events
        TgBot::TgLongPoll longPoll(bot);

        while (true)
        {
            longPoll.start();
        }
    } catch (const std::exception& ex)
    {
	fmt::print(stderr, "Error: {}\n", ex.what());
        std::exit(EXIT_FAILURE);
    }

    return 0;
}
