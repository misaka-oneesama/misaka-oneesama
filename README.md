<img src="https://magiruuvelvet.s-ul.eu/pics/5ANbjzVE.png" height="200" alt="" align="left">

# 御坂ーお姉さま
<sup>*Misaka-oneesama*</sup>

Discord Bot with Web UI and Plugin support written in C++14

[![Discord Server](https://discordapp.com/api/guilds/238054360637112321/embed.png)](https://discord.gg/4dpCQXv)
<br><br>

**FreeBSD** [![FreeBSD Build Status](https://jenkins.magiruuvelvet.gdn/job/misaka-oneesama/job/misaka-oneesama/job/master/badge/icon)](https://jenkins.magiruuvelvet.gdn/job/misaka-oneesama/job/misaka-oneesama/job/master/) | **Linux** [![Linux Build Status](https://travis-ci.org/misaka-oneesama/misaka-oneesama.svg?branch=master)](https://travis-ci.org/misaka-oneesama/misaka-oneesama)


## Overview

**ATTENTION: In development!**

Misaka-oneesama is an upcoming bot for Discord™ with a Web-based configuration panel and plugin support. It will provide a simple Plugin API and a graphical configuration. The web interface is accessible via HTTP.

I plan to develop some plugins by myself to use with the bot.


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

**Overview:**

Instead of multithreading all components in one process I decided to try something with IPC instead. I want to have at least 3 separate processes - the `master` process which acts as controller and observer, the `server` process which handles all HTTP requests and operates the Web UI, and the `bot` process which is the actual Discord™ bot.

<sub>Note: There is still multithreading in all of the individual processes.</sub>

I also plan to give D-Bus a try to communicate between the processes.

##### What works?

**`A:`** The self spawning to create the other 2 instances as well as transferring the configuration to the child processes so that is loads only once during the initial startup. All components work and operate normally, but there isn't any communication yet between the childs. The master process detects when a child crashes but it doesn't respawn it yet. Sending SIGINT, SIGTERM or SIGQUIT to any of the child processes stops them after some cleanup work. Sending any of this signals to the master process stops any child processes (cleanup work is done first) and then it stops itself and finishes with the cleanup. SIGKILL is evil, but the app can be restarted without any critical problems so far. This changes in the future when I need more complex locks (`QSystemSemaphore`).

--

After running the bot, its Web UI can be accessed using [http://127.0.0.1:4555/](http://127.0.0.1:4555/) (default configuration). There are multiple processes. The server instance which handles HTTP requests and responses, the bot instance which communicates with the Discord™ API over WebSockets and then there is the master process which glues everything together.

At the moment there isn't anything interesting served over HTTP. Just some string saying `It's working :D` or an attempt to serve a file from the home directory on any other requested path. Discord events are processed in real-time and QDiscord supports quite some events already to make a useful bot.

To see everything in action make sure to run the bot from within a terminal.

Logs can be found in `$XDG_CONFIG_HOME/御坂ーお姉さま/logs` (fallback `$HOME/.config/御坂ーお姉さま/logs`).

## TODO list

 - [ ] Implement Bot Core
   - [x] Discord login and WebSocket communication
   - [ ] Implement a event handler for all the different Discord events
 - [ ] ~~Implement a solid multithreaded environment **[QThread and Signal/Slots]**~~ <br>
       Implement a solid IPC environment which communicates over D-Bus
     - [x] Basic Process Manager
     - [ ] D-Bus interfaces
     - [ ] Error handling and process respawning
     - [ ] Testing if everything works...
   - [x] Handle UNIX signals ~~(use C preprocessor to exclude Windows)~~
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
