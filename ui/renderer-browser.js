function createBrowserApp()
{
    const primaryColor = "#996C54";
    const secondaryColor = "#E5CFC3";
    const accentColor = "#C0876A";
    const textBgColor = "#F2E7E1";
    const neutralBgColor = "#F8F8F8";

    const app_container = document.getElementById("app-container");
    app_container.style.backgroundColor = neutralBgColor;

    const app = document.createElement('div');
    app.setAttribute('id', 'app');
    app.style.position = "relative";
    app.style.width = "100%";
    app.style.height = "100%";
    app.style.backgroundColor = neutralBgColor;

    app_container.appendChild(app);
}

document.addEventListener('DOMContentLoaded', function()
{
    createBrowserApp();
});
