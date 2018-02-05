<img src="https://magiruuvelvet.s-ul.eu/pics/5ANbjzVE.png" height="200" alt="" align="left">

# 御坂ーお姉さま
<sup>*Misaka-oneesama*</sup>

Discord Bot with Web UI, HTTP API, D-Bus Integration and Plugin support written in C++14

[![Discord Server](https://discordapp.com/api/guilds/277765099807047681/embed.png)](https://discord.gg/BvyFy5k)
<br><br>

**FreeBSD** [![FreeBSD Build Status](https://jenkins.magiruuvelvet.gdn/job/misaka-oneesama/badge/icon)](https://jenkins.magiruuvelvet.gdn/job/misaka-oneesama/lastBuild/console) | **Linux** [![Linux Build Status](https://travis-ci.org/misaka-oneesama/misaka-oneesama.svg?branch=master)](https://travis-ci.org/misaka-oneesama/misaka-oneesama)


## Overview

**ATTENTION: In development!**

Misaka-oneesama is an upcoming bot for Discord™ with a Web-based configuration panel and plugin support. It will provide a simple Plugin API and a graphical configuration. The web interface and API is accessible via HTTP. The D-Bus integration offers great possibilities to control the bot from 3rd party applications and scripts.

I plan to develop some plugins by myself to use with the bot.


### Features (**NOTICE:** subject to change during development)

 **`＊`** **IPC (Interprocess Communication) and multi-threaded** <br>
 Load balancing across multiple processes and threads for best performance and a non-blocking operation.
 <br><br>

 **`＊`** **Plugin Support** <br>
 for an endless extension of the bot. The sky is the limit. As of the current state Qt/C++ is the only planned supported language for Plugins. Once I learned more about language bindings I can think about adding support for other languages to the Plugin API.
 <br><br>

 **`＊`** **Web UI** <br>
 Graphical Configuration and a `REST`ful API accessible via HTTP. Remotely control the bot from everywhere.
 <br><br>

 **`＊`** **D-Bus Integrated** <br>
 Easily communicate with the bot over D-Bus calls and change its configuration.
<br><br>


## Requirements

This are the requirements and dependencies which are required to build and operate the bot. Those may be subject to change during development.

 - Linux, \*BSD (FreeBSD), macOS or any other UNIX like system <br>
   <sup>Cygwin on Windows may work too (untested and not recommended)</sup>
 - Recommended: 64-bit processor <br>
   Discord IDs are handled as actual *64-bit unsigned integers* internally and not as strings as the API returns them. <br>
   <sub>If you plan to operate this bot on a 32-bit RPi don't blame me for bad performance :P</sub>
 - UTF-8 compatible system environment and file system (**important!**)
   - This software makes use of UTF-8 characters in both, runtime and file system I/O
 - C++14 compiler
 - The [**D-Bus Message Bus**](https://dbus.freedesktop.org/)

 - [Qt](https://www.qt.io) 5.6+ (lower versions may work too, but not recommended)
   - Qt Core
   - Qt Concurrent ─ for asynchronous and non-blocking plugin execution in its own thread ***[experimental]***
   - Qt SQL (with the `QSQLITE` driver) ─ for the configuration store
   - Qt D-Bus ─ for the Interprocess communication (IPC)
   - Qt Network ─ for the HTTP server which operates the Web UI
   - Qt WebSockets ─ for the Discord API communication


##### Things used to build this Bot

 - [Qt](https://www.qt.io)
 - [QDiscord](https://github.com/george99g/QDiscord) ─ Qt C++ Discord API wrapper by [george99g](https://github.com/george99g)
 - [QtWebApp](http://stefanfrings.de/qtwebapp/index-en.html) ─ Qt C++ HTTP Server by [Stefan Frings](http://stefanfrings.de)


##### Note about Support Platforms

All Qt supported UNIX like platforms should be supported. Originally this is a personal bot and it needs to work mainly on FreeBSD only, which is the OS of my choice for servers. If there are any issues on other OSes feel free to fix it yourself and make a *pull request* or open an issue and let me know. Also please don't ask for Windows support, thanks. You can try Cygwin or other UNIX compatibility layers on Windows; you are on your own though - I can't assist you with that.


## Detailed Description (this notes can become outdated very often during developement)

After running the app it checks if a D-Bus connection can be established. On success it self-spawns itself with different command line arguments to start all the components which again checks for a D-Bus connection and the interface the master process created. The master interface is required for the child processes, if it is not available (example: on direct execution, skipping the master process) the component refuses to start and kills itself. There are 3 processes in total. The first is the master process which glues all components together and must be running. The second is the Server which operates the Web UI. The Web UI can be accessed using [http://127.0.0.1:4555/](http://127.0.0.1:4555/) (default configuration). The server instance handles all HTTP requests and responses and is multithreaded and can handle hundreds of requests in parallel. The third is the actual Discord™ bot which communicates with the WebSocket connection is realtime. The bot instance makes use of Qt Concurrent to execute all events for every plugin in its own thread in a non-blocking way - this is currently highly experimental and can cause hundreds of threads to be spawned (more plugins = more threads).

#### D-Bus

All 3 instances have their own D-Bus service and methods. I recommend you fire up the **Qt QDBusViewer** application for a convenient and complete list of all current methods. To call a method from the command line you can use `qdbus` or `dbus-send`.

`qdbus moe.misaka_oneesama / {methodName} {args}` <br>
`dbus-send --session --dest=moe.misaka_oneesama --type=method_call --print-reply=literal / moe.misaka_oneesama.{methodName} {args}`

**Example 1:** Check if the bot process is running.

`qdbus moe.misaka_oneesama / isBotRunning` <br>
`dbus-send --session --dest=moe.misaka_oneesama --type=method_call --print-reply=literal / moe.misaka_oneesama.isBotRunning`

**Example 2:** Send a message to an channel.

`qdbus moe.misaka_oneesama.Bot / sendMessage {channel-id} {message-body}` <br>
`dbus-send --session --dest=moe.misaka_oneesama.Bot --type=method_call --print-reply=literal / moe.misaka_oneesama.sendMessage uint64:{channel-id} string:{message-body}`


There are several methods already, including but not limited to

  - Start/Stop/Halt/Restart/Reload the server process
  - Start/Stop/Restart the bot process
  - Check if a component is running or not
  - Change the server configuration at runtime and reload it

There will be many more methods in the future. Most of them will be used programmatically in the future to control and configure most things. This is already the planned and preferred method to configure the bot from the Web UI and API. Some methods will be unused internally, but they offer a great possibility to control the bot from scripts or 3rd party applications via the D-Bus daemon. This is one thing which will make this a very powerful and highly configurable Discord™ bot :)

#### Plugins

The only supported language to write plugins is C++ together with Qt. When everything is more complete I may give language bindings a try. There is no plugin loader for external plugins yet. To make an built-in plugin just inherit the abstract `PluginInterface` class and `override` all methods you need to the plugin. Plugins aren't thread-safe, you must use mutexes where needed.


#### Developer Notes

At the moment there isn't anything interesting served over HTTP. Just some string saying `It's working 👌` or a 404 error on any other requested path. The API can be found under `/api`. See `Source/Server/RequestMapper.cpp` and `Source/Server/API` for more details about the implemented endpoints. The API returns JSON responses, at the moment those aren't very helpful. Discord events are processed in real-time and QDiscord supports quite some events already to make a useful bot. I forked this library and I may make some changes to it as I need them.

To see everything in action make sure to run the bot from within a terminal. Quick side note: Even when the output is handled by the master process and is protected by a mutex, in rare cases the output still can become very cluttered at some point. I'm investigating why this happens.

Logs can be found in `$XDG_CONFIG_HOME/御坂ーお姉さま/logs` (fallback `$HOME/.config/御坂ーお姉さま/logs`).

## TODO list

 - [ ] Refactoring and general improvements, fix lazy spaghetti code from early development!!
   - [x] Refactor `ConfigManager` to use SQLite3 instead of raw binary serialization
   - [x] Proper and correct command line parsing. > Hardcoding argument positions -> totally not lazy code ¯\\_(ツ)_/¯
     - [x] Master instance
     - [x] Child instances
   - [x] Refactor `main.cpp`. Toooooo much code redundancy, inefficient and unreadable.
   - [x] Refactor and improve `Server` and `BotManager` signals and slots and overall structure.
     - [x] `Server` and `RequestMapper`
     - [ ] `BotManager`
 - [ ] Implement Bot Core
   - [x] Discord login and WebSocket communication
   - [ ] Implement a event handler for all the different Discord events
 - [ ] ~~Implement a solid multithreaded environment **[QThread and Signal/Slots]**~~ <br>
       Implement a solid IPC environment which communicates over D-Bus
     - [x] Basic Process Manager
     - [x] D-Bus interfaces
     - [ ] Error handling and process respawning
     - [ ] Testing if everything works...
 - [x] Handle UNIX signals ~~(use C preprocessor to exclude Windows)~~
   - [x] Clean up and process termination
 - [x] Implement basic HTTP server
   - [x] Improve QtWebApp (`HttpServer` module)
     - [x] use alternative config store [`QSettings` spams unnecessary useless files to disk]
     - [x] get rid of unnecessary variable copying
     - [x] use C++14 features, get rid of ANSI C - C++ hybrid code
     - [x] improve code readability [reformat coding style]
     - [x] significantly improve the static file controller and make it more flexible <br>
           the current implementation has a hardcoded incomplete and ugly mime type list <br>
           the use of `QMimeDatabase` sounds like a good idea here :)
   - [ ] Implement basic Web UI
     - [ ] Implement actual Web UI with functionality
 - [ ] Implement Plugin API
   - [ ] Implement config store for plugins, one file per plugin
   - [ ] Implement a plugin loader to make use of external plugins
 - [ ] Implement some built-in plugins
   - [ ] Chat commands with simple text responses
   - [ ] Server Info, User Info, Role Info, ...
   - [ ] and much more (all plugins can be completely disabled and will never be copied into memory or affect runtime performance)

<br>

--

## Copyright Notices

- **QDiscord** is licensed under the terms and conditions of the `LGPL-3.0`
- **QtWebApp** is licensed under the terms and conditions of the `LGPL-3.0`

The character 美琴**御坂** (Mikoto Misaka) <sup>[Picture]</sup> is a fictional anime character created by 鎌池和馬 (Kamachi Kazuma). She is a main character in the anime とある科学の超電磁砲（レールガン）.
