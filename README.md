<img src="https://magiruuvelvet.s-ul.eu/pics/5ANbjzVE.png" height="200" alt="" align="left">

# 御坂ーお姉さま
<sup>*Misaka-oneesama*</sup>

Discord Bot with Web UI and Plugin support written in C++14

[![Discord Server](https://discordapp.com/api/guilds/238054360637112321/embed.png)](https://discord.gg/4dpCQXv)
<br><br>

**FreeBSD** [![FreeBSD Build Status](https://jenkins.magiruuvelvet.gdn/job/misaka-oneesama/job/misaka-oneesama/job/master/badge/icon)](https://jenkins.magiruuvelvet.gdn/job/misaka-oneesama/job/misaka-oneesama/job/master/) | **Linux** [![Linux Build Status](https://travis-ci.org/misaka-oneesama/misaka-oneesama.svg?branch=master)](https://travis-ci.org/misaka-oneesama/misaka-oneesama)


## Overview

**ATTENTION: In development!**

Misaka-oneesama is an upcoming bot for Discord™ with a Web-based configuration panel and plugin support. It will provide a simple Plugin API and a graphical configuration. The web interface is accessible via HTTP. The D-Bus integration offers great possibilities to control the bot from 3rd party applications and scripts.

I plan to develop some plugins by myself to use with the bot.


### Features (**NOTICE:** incomplete list, subject to change during development)

 - **IPC and multithreaded** - load balancing across multiple processes and threads
 - **Web UI** to setup and configure the bot
 - **D-Bus integrated** - control the bot from 3rd party scripts and applications


## Requirements

This are the requirements and dependencies which are required to build and operate the bot. Those may be subject to change during development.

 - Linux, \*BSD (FreeBSD), macOS or any other UNIX like system <br>
   <sup>Cygwin on Windows may work too (untested and not recommended)</sup>
 - UTF-8 compatible system environment and file system (**important!**)
   - This software makes use of UTF-8 characters in both, runtime and file system I/O
 - C++14 compiler

 - [Qt](https://www.qt.io) 5.6+ (lower versions may work too, but not recommended)
   - Qt Core
   - Qt D-Bus ─ for the Interprocess communication (IPC)
   - Qt Network ─ for the HTTP server which operates the Web UI
   - Qt WebSockets ─ for the Discord API communication


##### Things used to build this Bot

 - [Qt](https://www.qt.io)
 - [QDiscord](https://github.com/george99g/QDiscord) ─ Qt C++ Discord API wrapper by [george99g](https://github.com/george99g)
 - [QtWebApp](http://stefanfrings.de/qtwebapp/index-en.html) ─ Qt C++ HTTP Server by [Stefan Frings](http://stefanfrings.de)


##### Note about Support Platforms

All Qt supported UNIX like platforms should be supported. Originally this is a personal bot and it needs to work mainly on FreeBSD only, which is the OS of my choice for servers. If there are any issues on other OSes feel free to fix it yourself and make a *pull request* or open an issue and let me know. Also please don't ask for Windows support, thanks.


## Testing

After running the app it checks if a D-Bus connection can be established. On success it self-spawns itself with different command line arguments to start all the components which again checks for a D-Bus connection and the interface the master process created. The master interface is required for the child processes, if it is not available (example: on direct execution, skipping the master process) the component refuses to start and kills itself. There are 3 processes in total. The first is the master process which glues all components together and must be running. The second is the Server which operates the Web UI. The Web UI can be accessed using [http://127.0.0.1:4555/](http://127.0.0.1:4555/) (default configuration). The server instance handles all HTTP requests and responses and is multithreaded and can handle hundreds of requests in parallel. The third is the actual Discord™ bot which communicates with the WebSocket connection is realtime.

#### D-Bus

All 3 instances have their own D-Bus service and methods. I recommend you fire up the **Qt QDBusViewer** application for a convenient and complete list of all methods. To call a method from the command line you can use `qdbus` or `dbus-send`.

`qdbus moe.misaka_oneesama / {methodName}` <br>
`dbus-send --session --dest=moe.misaka_oneesama --type=method_call --print-reply=literal / moe.misaka_oneesama.{methodName}`

**Example:** Check if the bot process is running.

`qdbus moe.misaka_oneesama / isBotRunning` <br>
`dbus-send --session --dest=moe.misaka_oneesama --type=method_call --print-reply=literal / moe.misaka_oneesama.isBotRunning`


There are several methods already, including but not limited to

  - Start/Stop/Restart/Reload the server process
  - Start/Stop/Restart the bot process
  - Check if a component is running or not
  - Change the server configuration at runtime and hot reload it

There will be many more methods in the future. Most of them will be used programmatically in the future to control and configure most things. This is already the planned and preferred method to configure the bot from the Web UI. Some methods will be unused internally, but they offer a great possibility to control the bot from scripts or 3rd party applications via the D-Bus daemon. This is one thing which will make this a very powerful and highly configurable Discord™ bot :)

#### Notes

At the moment there isn't anything interesting served over HTTP. Just some string saying `It's working :D` or an attempt to serve a file from the home directory on any other requested path. Discord events are processed in real-time and QDiscord supports quite some events already to make a useful bot.

To see everything in action make sure to run the bot from within a terminal.

Logs can be found in `$XDG_CONFIG_HOME/御坂ーお姉さま/logs` (fallback `$HOME/.config/御坂ーお姉さま/logs`).

## TODO list

 - [ ] Refactor `ConfigManager` to use SQLite3 instead of raw binary serialization
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
 - [ ] Implement some plugins
   - [ ] Chat commands with simple text responses
   - [ ] Server Info, User Info, Role Info, ...

<br>

## Copyright Notices

- **QDiscord** is licensed under the terms and conditions of the `LGPL-3.0`
- **QtWebApp** is licensed under the terms and conditions of the `LGPL-3.0`

The character 美琴**御坂** (Mikoto Misaka) <sup>[Picture]</sup> is a fictional anime character created by 鎌池和馬 (Kamachi Kazuma). She is a main character in the show とある科学の超電磁砲（レールガン）.
