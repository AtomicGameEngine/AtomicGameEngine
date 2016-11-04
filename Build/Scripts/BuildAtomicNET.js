var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var os = require('os');
var config = require('./BuildConfig');

var atomicRoot = config.atomicRoot;

namespace('build', function() {

    task('atomicnetdocs', {
        async: true
    }, function() {

        var assemblyFolder = atomicRoot + "Artifacts/AtomicNET/" + config["config"] + "/Desktop/";
        var templateFile = atomicRoot + "Build/Docs/AtomicNET/atomictemplate.xsl";
        var sourceAssembly = assemblyFolder + "AtomicNET.dll";
        var sourceAssemblyXML = assemblyFolder + "AtomicNET.xml";

        var outputFolder = atomicRoot + "Artifacts/AtomicNET/Docs/";

        // Make sure the assemblies exist, this is only an issue when running the task
        // directly with --task=build:atomicnetdocs
        if (!fs.existsSync(sourceAssembly) || !fs.existsSync(sourceAssemblyXML)) {
            throw new Error("\n\nSource assembly doesn't exist in: " + assemblyFolder + "\nPlease try specifying --debug?\n\n");
        }

        host.cleanCreateDir(outputFolder)

        var cmds = [];

        cmds.push("mdoc update -i \"" + sourceAssemblyXML + "\" \"" + sourceAssembly +
        "\" -o \"" + outputFolder + "xml\"" );

        // mdoc export-html --template=atomictemplate.xsl -o html xml
        cmds.push("mdoc export-html --template=\"" + templateFile +
        "\" -o \"" + outputFolder +"html\" \"" + outputFolder + "xml\"");

        jake.exec(cmds, function() {

            complete();

        }, {
            printStdout: true,
            printStderr: true
        });


    });


    task('atomicnet', {
        async: true
    }, function() {


        platforms = "-platform desktop";

        if (config["with-android"])
            platforms += " -platform android";

        if (config["with-ios"])
            platforms += " -platform ios";

        var cmds = [];

        var netCmd = host.atomicTool + " net compile " + atomicRoot + "Script/AtomicNET/AtomicNETProject.json " + platforms + " -config " + config["config"] + " -toolbootstrap";

        console.log(netCmd);

        cmds.push(netCmd);

        jake.exec(cmds, function() {

            complete();

        }, {
            printStdout: true,
            printStderr: true
        });

    });

}); // end of build namespace
