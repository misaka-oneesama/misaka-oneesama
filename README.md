<img src="https://magiruuvelvet.s-ul.eu/pics/5ANbjzVE.png" height="200" alt="" align="left">

# 御坂ーお姉さま
<sup>*Misaka-oneesama*</sup>

Discord Bot with Web UI and Plugin support written in C++14

[![Discord Server](https://discordapp.com/api/guilds/238054360637112321/embed.png)](https://discord.gg/4dpCQXv)
<br><br>

**FreeBSD** [![FreeBSD Build Status](https://jenkins.magiruuvelvet.gdn/job/misaka-oneesama/job/misaka-oneesama/job/master/badge/icon)](https://jenkins.magiruuvelvet.gdn/job/misaka-oneesama/job/misaka-oneesama/job/master/) | **Linux** [![Linux Build Status](https://travis-ci.org/misaka-oneesama/misaka-oneesama.svg?branch=master)](https://travis-ci.org/misaka-oneesama/misaka-oneesama) | **Windows** [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/misaka-oneesama/misaka-oneesama?svg=true)](https://ci.appveyor.com/project/GhettoGirl/misaka-oneesama)


## Overview

**ATTENTION: In development!**

Misaka-oneesama is an upcoming bot for Discord™ with a Web-based configuration panel and plugin support. It will provide a simple Plugin API and a graphical configuration. The web interface is accessible via HTTP.

I plan to develop some plugins by myself to use with the bot.


## Requirements

This are the requirements and dependencies which are required to build and operate the bot. Those may be subject to change during development.

 - UTF-8 compatible system environment and file system (**important!**)
   - This software makes use of UTF-8 characters in both, runtime and file system I/O
 - C++14 compiler
 - Any Qt supported Operating System (Linux, \*BSD, macOS, Windows, etc.)

 - [Qt](https://www.qt.io) 5.6+ (lower versions may work too, but not recommended)
   - Qt Core
   - Qt Network ─ for the HTTP server which operates the Web UI
   - Qt WebSockets ─ for the Discord API communication


##### Things used to build this Bot

 - [Qt](https://www.qt.io)
 - [QDiscord](https://github.com/george99g/QDiscord) ─ Qt C++ Discord API wrapper by [george99g](https://github.com/george99g)
 - [QtWebApp](http://stefanfrings.de/qtwebapp/index-en.html) ─ Qt C++ HTTP Server by [Stefan Frings](http://stefanfrings.de)


##### Note about Support Platforms

All Qt supported platforms should be supported. Originally this is a personal bot and it needs to work mainly on FreeBSD only, which is the OS of my choice for servers. If there are any issues on the OS of your choice feel free to fix it yourself and make a *pull request* or open an issue and let me know.


## Testing

After running the bot, its Web UI can be accessed using [http://127.0.0.1:4555/](http://127.0.0.1:4555/) (default configuration). There are multiple threads and the bot is capable of handling requests and responses from both, HTTP and Discord™ WebSockets.

At the moment there isn't anything interesting served over HTTP. Just some string saying `It's working :D` or a 404 error message on any other requested path. Discord events are processed in real-time and QDiscord supports quite some events already to make a useful bot.

To see everything in action make sure to run the bot from within a terminal.


## TODO list

 - [ ] Implement Bot Core
   - [x] Discord login and WebSocket communication
   - [ ] Implement a event handler for all the different Discord events
 - [ ] Implement a solid multithreaded environment **[QThread and Signal/Slots]**
   - [x] Handle UNIX signals (use C preprocessor to exclude Windows)
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
