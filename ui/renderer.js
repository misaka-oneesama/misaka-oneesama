const {remote, ipcRenderer} = require('electron');
const rWindow = remote.getCurrentWindow();

let btnMaximize;

function createTitleBar()
{
    const appName = document.createElement('span');
    appName.innerHTML = rWindow.getTitle();

    const appIcon = document.createElement('img');
    appIcon.setAttribute('src', '../assets/icon.png');
    appIcon.setAttribute('height', '26');
    appIcon.setAttribute('align', 'right');
    appIcon.style.float = "right";
    appIcon.style.clear = "right";

    const btnClose = document.createElement('button');
    btnClose.setAttribute('type', 'button');
    btnClose.style.backgroundImage = "url(../assets/close.svg)";
    btnClose.style.backgroundRepeat = "no-repeat";
    btnClose.style.backgroundSize = "16px 16px";
    btnClose.style.backgroundPosition = "5px 5px";
    btnClose.style.backgroundColor = "rgb(200, 0, 0)";

    const btnCloseFocusOut = function()  { btnClose.style.backgroundColor = "rgb(200, 0, 0)"; };
    const btnCloseFocus = function()     { btnClose.style.backgroundColor = "rgb(225, 0, 0)"; };
    const btnCloseHoverOut = function()  { btnClose.style.backgroundColor = "rgb(200, 0, 0)"; };
    const btnCloseHover = function()     { btnClose.style.backgroundColor = "rgb(230, 0, 0)"; };
    const btnCloseMouseDown = function() { btnClose.style.backgroundColor = "rgb(250, 0, 0)"; };
    const btnCloseMouseUp = function()   { btnClose.style.backgroundColor = "rgb(200, 0, 0)"; };

    btnClose.addEventListener('focusout', btnCloseFocusOut);
    btnClose.addEventListener('mouseout', btnCloseHoverOut);
    btnClose.addEventListener('focus', btnCloseFocus);
    btnClose.addEventListener('mouseover', btnCloseHover);
    btnClose.addEventListener('mousedown', btnCloseMouseDown);
    btnClose.addEventListener('mouseup', btnCloseMouseUp);

    btnClose.addEventListener('click', function()
    {
        remote.app.quit();
    });

    btnMaximize = document.createElement('button');
    btnMaximize.setAttribute('type', 'button');
    btnMaximize.style.backgroundRepeat = "no-repeat";
    btnMaximize.style.backgroundSize = "12px 12px";
    btnMaximize.style.backgroundPosition = "9px 9px";
    btnMaximize.style.backgroundColor = "transparent";
    btnMaximize.style.opacity = 0.7;

    const btnMaximizeFocusOut = function()  { btnMaximize.style.opacity = 0.7; };
    const btnMaximizeFocus = function()     { btnMaximize.style.opacity = 0.8; };
    const btnMaximizeHoverOut = function()  { btnMaximize.style.opacity = 0.75; };
    const btnMaximizeHover = function()     { btnMaximize.style.opacity = 0.85; };
    const btnMaximizeMouseDown = function() { btnMaximize.style.opacity = 0.9; };
    const btnMaximizeMouseUp = function()   { btnMaximize.style.opacity = 0.7; };

    btnMaximize.addEventListener('focusout', btnMaximizeFocusOut);
    btnMaximize.addEventListener('mouseout', btnMaximizeHoverOut);
    btnMaximize.addEventListener('focus', btnMaximizeFocus);
    btnMaximize.addEventListener('mouseover', btnMaximizeHover);
    btnMaximize.addEventListener('mousedown', btnMaximizeMouseDown);
    btnMaximize.addEventListener('mouseup', btnMaximizeMouseUp);

    btnMaximize.addEventListener('click', toggleWindowState);

    const btnMinimize = btnMaximize.cloneNode();
    btnMinimize.style.backgroundImage = "url(../assets/minimize.svg)";

    const btnMinimizeFocusOut = function()  { btnMinimize.style.opacity = 0.7; };
    const btnMinimizeFocus = function()     { btnMinimize.style.opacity = 0.8; };
    const btnMinimizeHoverOut = function()  { btnMinimize.style.opacity = 0.75; };
    const btnMinimizeHover = function()     { btnMinimize.style.opacity = 0.85; };
    const btnMinimizeMouseDown = function() { btnMinimize.style.opacity = 0.9; };
    const btnMinimizeMouseUp = function()   { btnMinimize.style.opacity = 0.7; };

    btnMinimize.addEventListener('focusout', btnMinimizeFocusOut);
    btnMinimize.addEventListener('mouseout', btnMinimizeHoverOut);
    btnMinimize.addEventListener('focus', btnMinimizeFocus);
    btnMinimize.addEventListener('mouseover', btnMinimizeHover);
    btnMinimize.addEventListener('mousedown', btnMinimizeMouseDown);
    btnMinimize.addEventListener('mouseup', btnMinimizeMouseUp);

    btnMinimize.addEventListener('click', function()
    {
        rWindow.minimize();
    });

    // add buttons
    const titleBarButtons = document.getElementById("window-decoration-title-bar-buttons");
    titleBarButtons.appendChild(btnClose);
    titleBarButtons.appendChild(btnMaximize);
    titleBarButtons.appendChild(btnMinimize);

    // add other elements
    const titleBarInner = document.getElementById("window-decoration-title-bar-inner");
    titleBarInner.appendChild(appName);
    titleBarInner.appendChild(appIcon);
}

function toggleWindowState()
{
    if (rWindow.isMaximized())
    {
        rWindow.restore();
        btnMaximize.style.backgroundImage = "url(../assets/maximize.svg)";
    }

    else
    {
        rWindow.maximize();
        btnMaximize.style.backgroundImage = "url(../assets/restore.svg)";
    }
}

function colorizeWindowDecoration()
{
    const primaryColor = "#996C54";
    const secondaryColor = "#E5CFC3";
    const accentColor = "#C0876A";
    const textBgColor = "#F2E7E1";
    const neutralBgColor = "#F8F8F8";

    document.getElementById("window-decoration-border").style.border = "none";
    document.getElementById("window-decoration-border").style.width = "100%";
    document.getElementById("window-decoration-border").style.height = "100%";
    document.getElementById("window-decoration-title-bar").style.backgroundColor = primaryColor;
    document.getElementById("window-decoration-app-container").style.background = neutralBgColor;
}

document.addEventListener('DOMContentLoaded', function()
{
    createTitleBar();
    rWindow.isMaximized() ?
        btnMaximize.style.backgroundImage = "url(../assets/restore.svg)"
      : btnMaximize.style.backgroundImage = "url(../assets/maximize.svg)";
    rWindow.on('maximize', ()=> btnMaximize.style.backgroundImage = "url(../assets/restore.svg)");
    rWindow.on('unmaximize', ()=> btnMaximize.style.backgroundImage = "url(../assets/maximize.svg)");

    colorizeWindowDecoration();

    const webview = document.createElement('webview');
    webview.setAttribute('id', 'app');
    webview.setAttribute('src',
        'http://' + remote.getGlobal('config').ServerAddress + ':' + remote.getGlobal('config').ServerPort);
    webview.style.height = "calc(100% - 6px + 40px)";
    webview.style.width = "calc(100% + 40px)";
    webview.style.margin = "-20px";
    document.getElementById("window-decoration-app-container").appendChild(webview);
});
