# A reactive Telegram bot

This is an example of a C++ telegram bot using a reactive API. See [this
repository](https://github.com/joaquintides/usingstdcpp2019) for details.

# How it works?

The `usingstdcpp2019::urp` API implements an event stream API similar to
[Rx](http://reactivex.io/). This bot is an example of integrating that API
into the [tgbot_cpp API](https://github.com/reo7sp/tgbot-cpp) to compose
powerful Telegram message handling mechanisms. To do so, the example
`bop.cpp` implements a `ReactiveBot` class that subscribes to any user
message sent to the Telegram bot with
`TgBot::EventBroadcaster::onAnyMessage()` and sends them through the
`urp::trigger` API to generate an event. From there the message is an
event that can be streamed and combined using different operators.

# How to build

The example uses conan to gather all dependencies, which are all linked
statically to generate a deps-free standalone executable. To set up the
project run the following commands from the source tree:

``` shell
$ mkdir build && cd build
$ conan install .. --build=missing
$ cmake ..
$ make
```

> You need the following conan repositories:
>  - **Manu343726** (For `tgbot_cpp` and `conan_common_recipes`): https://api.bintray.com/conan/manu343726/conan-packages
>  - **bincrafters** (For `boost`, `fmt`, etc): https://api.bintray.com/conan/bincrafters/public-conan
>
> The repositories are configured in your machine as follows:
>
> ``` shell
> $ conan remote add Manu343726 https://api.bintray.com/conan/manu343726/conan-packages
> $ conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
> ```

If everything runs smoothly there should be a `bot` executable in
`build/bin/`.

# Running the bot

Telegram bots use an unique API token to identify themselves. This token
should be given to the bot through an environment variable called
`USINGSTDCPP2019_BOT_TOKEN`:

``` shell
$ export USINGSTDCPP2019_BOT_TOKEN="your telegram bot API token"
$ ./bin/bot
Hi! I'm @usingstdcpp2019_bot, I'm ready to talk with you!
```


See [the Telegram Bot docs](https://core.telegram.org/api) to get a new
API token.
