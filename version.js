/* Software Versioning Scheme Parser for NodeJS
 *
 * This file is part of my custom software versioning scheme.
 * Read more about it at
 * "github.com/GhettoGirl/common/blob/master/docs/Software Versioning Scheme.md"
 *
 * Licensed under the MIT license.
 *
 */

const path = require('path');

function pkg_version_error(msg)
{
    console.error("version: " + msg);
}

const method = pkg_version.prototype;

function pkg_version(package_json)
{
    var pjson;

    if (typeof package_json == "string")
    {
        // use provided package.json file
        try
        {
            pjson = require(path.join(__dirname, package_json));
        }

        catch (error)
        {
            pjson = undefined;
        }
    }

    else
    {
        // use the application's package.json file
        try
        {
            const app_dir = path.dirname(require.main.filename);
            pjson = require(app_dir + "/package.json");
        }

        catch (error)
        {
            pjson = undefined;
        }
    }

    if (pjson !== undefined)
    {
        var ver = pjson.version.split('.');

        if (ver.length != 3)
        {
            pkg_version_error("No valid semantic version string found in package.json");
            this.m_main = 0;
            this.m_progress = 0;
            this.m_revision = 0;
        }

        else
        {
            if (ver[1].length != 1 || ver[2].length != 1 || pjson.revision.indexOf('.') != -1)
            {
                pkg_version_error("Invalid version string");
                this.m_main = 0;
                this.m_progress = 0;
                this.m_revision = 0;
            }

            else
            {
                this.m_main = ver[0];
                this.m_progress = ver[1] + ver[2];
                this.m_revision = pjson.revision;
            }
        }
    }

    else
    {
        pkg_version_error("No package.json file to parse");
    }
}

method.version = function()
{
    return this.m_main + "." + this.m_progress + "-" + this.m_revision;
}

method.main = function()
{
    return this.m_main;
}

method.progress = function()
{
    return this.m_progress;
}

method.revision = function()
{
    return this.m_revision;
}

module.exports = pkg_version;
