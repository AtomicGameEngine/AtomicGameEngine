var fs = require('fs-extra');
var path = require("path");
var os = require('os');
var glob = require("glob");
var config = require("./BuildConfig");

var fixTabs = false;

var atomicRoot = config.atomicRoot;

// Oh, JavaScript
atomicRoot = atomicRoot.split("\\");
atomicRoot = atomicRoot.join("/");

function lintFile(filename) {

    var file = fs.readFileSync(filename, "utf-8");

    if (file.indexOf('\t') != -1) {

        console.log(filename + "  contains tab character");

        if (fixTabs) {

            var fd = fs.openSync(filename, "w");

            var tabbed = file.split('\t');
            file = tabbed.join("    ");

            fs.writeSync(fd, file);
            fs.closeSync(fd);
        }
    }

}

namespace('build', function() {

    task('lint', {
        async: true
    }, function() {

        // I looked into options and there are many for C# and C++, though simply want to catch tabs/spaces right now
        // and not introduce a dependency, soooooo

        pattern = [ atomicRoot + "Source/**/*.cpp",
                    atomicRoot + "Source/**/*.h",
                    atomicRoot + "Script/AtomicNET/**/*.cs"
                 ];

        pattern = "{" + pattern.join(",") + "}";

        // I failed at figure out glob exclusion patterns
        exclude = [atomicRoot + "Source/ThirdParty/",
                   atomicRoot + "Source/ToolCore/JSBind/cplusplus/"];

        var filenames = glob.sync(pattern);

        for (var i = 0; i < filenames.length; i++) {

            var filename = filenames[i];

            var skip = false;

            for (var j = 0; j < exclude.length; j++) {

                if (filename.startsWith(exclude[j])) {
                    skip = true;
                    break;
                }
            }

            if (skip)
                continue;

            lintFile(filename);
        }


        process.exit(0);
        complete();

    });

}); // end of build namespace
