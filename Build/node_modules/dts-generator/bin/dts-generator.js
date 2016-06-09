"use strict";
var index_1 = require('../index');
module.exports = function main(argv) {
    var kwArgs = {
        files: [],
        sendMessage: console.log.bind(console)
    };
    for (var i = 0; i < argv.length; ++i) {
        var arg = argv[i];
        if (arg.charAt(0) === '-') {
            var key = argv[i].replace(/^-+/, '');
            var value = argv[i + 1];
            ++i;
            if (key === 'exclude') {
                if (!kwArgs.exclude) {
                    kwArgs.exclude = [];
                }
                kwArgs.exclude.push(value);
            }
            else if (key === 'extern') {
                if (!kwArgs.externs) {
                    kwArgs.externs = [];
                }
                kwArgs.externs.push(value);
            }
            else if (key === 'verbose') {
                kwArgs.verbose = true;
                /* decrement counter, because vebose does not take a value */
                --i;
            }
            else {
                kwArgs[key] = value;
            }
        }
        else {
            kwArgs.files.push(argv[i]);
        }
    }
    ['out'].forEach(function (key) {
        if (!kwArgs[key]) {
            console.error("Missing required argument \"" + key + "\"");
            process.exit(1);
        }
    });
    if (!kwArgs.baseDir && !kwArgs.project) {
        console.error("Missing required argument of \"baseDir\" or \"project\"");
        process.exit(1);
    }
    if (!kwArgs.project && kwArgs.files.length === 0) {
        console.error('Missing files');
        process.exit(1);
    }
    console.log('Starting');
    return index_1.default(kwArgs).then(function () {
        console.log('Done!');
    });
};
