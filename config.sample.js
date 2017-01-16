module.exports = {

    // Insert your Bot's client ID and token here
    ClientID: "",
    ClientToken: "",

    // Insert your server's listening address here (default is strict localhost only)
    // Use '0.0.0.0' to listen on all interfaces
    ServerAddress: "127.0.0.1",

    // Insert port number of OAuth callback url here (default is 9876)
    ServerPort: 9876,

};

// choose wisely for your bot instance, this can't be changed after you invited your bot account to your server
// https://discordapp.com/api/oauth2/authorize?client_id={client_id}&scope=bot&permissions={permissions}
// https://discordapi.com/permissions.html
