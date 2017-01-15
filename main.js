const electronLocalShortcut = require('electron-localshortcut');
const {app, BrowserWindow} = require('electron');

const path = require('path');
const fs = require('fs');
const express = require('express');
const http = require('http');
const pjson = require(path.join(__dirname, 'package.json'));

require('rootpath')();
global.pkg_version = new (require(path.join(__dirname, 'version.js')));

let mainWindow;

function createWindow()
{
    console.log("Creating main window...");

    mainWindow = new BrowserWindow({
        title:       pjson.display_name,
        width:       700,
        height:      600,
        minWidth:    700,
        minHeight:   600,
        resizable:   true,
        frame:       false,
        icon:        path.join(__dirname, "assets", "icon.png")
    });
    mainWindow.loadURL(`file://${__dirname}/ui/index.html`);
    mainWindow.setMenu(null);

    electronLocalShortcut.register(mainWindow, 'CommandOrControl+Shift+I', function()
    {
        mainWindow.openDevTools();
    });

    electronLocalShortcut.register(mainWindow, 'F5', function()
    {
        mainWindow.reload();
    });

    mainWindow.on('closed', function()
    {
        mainWindow = null;
    });

    // prevent window title change
    mainWindow.on('page-title-updated', function(event, title)
    {
        event.preventDefault();
    });
}

function loadConfig()
{
    console.log("Loading configuration...");

    try
    {
        global.config = require(path.join(__dirname, 'config.js'));
    }

    catch (err)
    {
        console.error("Unable to load configuration! Make sure 'config.js' is valid.");
        process.exit(1);
    }
}

function startServer()
{
    console.log("Starting server...");

    global.expressApp = express();
    global.server = http.createServer(global.expressApp);

    global.server.listen(global.config.ServerPort, global.config.ServerAddress);
    global.server.on('listening', function()
    {
        console.log("Server started and listening on %s:%d", global.server.address().address, global.server.address().port);
        global.serverRunning = true;
    });

    global.expressApp.use('/', express.static('.'));
    global.expressApp.use('/styles', express.static('./styles'));
    global.expressApp.use('/renderer.js', express.static('./ui/renderer-browser.js'));

    global.expressApp.get('/', function(req, res)
    {
        res.sendFile(path.join(__dirname, "ui/web.html"));
    });
}

function stopServer()
{
    console.log("Stopping server...");

    global.server.close();
    global.serverRunning = false;

    console.log("Server stopped");
}

function init()
{
    console.log("%s %s\n", pjson.display_name, global.pkg_version.version());

    loadConfig();
    startServer();

    app.on('ready', function()
    {
        createWindow();
    });

    app.on('window-all-closed', function()
    {
        if (process.platform != 'darwin')
        {
            app.quit();
        }
    });

    app.on('activate', function()
    {
        if (mainWindow == null)
        {
            createWindow();
        }
    });

    app.on('quit', function()
    {
        stopServer();
    });
}

init();
