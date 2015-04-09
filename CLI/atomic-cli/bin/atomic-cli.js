#!/usr/bin/env node
"use strict";

// https://github.com/yeoman/update-notifier

// https://github.com/tj/commander.js
var path = require("path");
var fs = require("fs");
var program = require('commander');
var cli = require("atomic-cli")
var open = require("open");
var prompt = require('prompt');
var osenv = require('osenv')

prompt.message = "";
prompt.delimiter = "";

var saveAtomicConfig = function(activated) {

  var directory = osenv.home() + "/.atomicgameengine";

  if (!fs.existsSync(directory)) {
    fs.mkdir(directory);
  }

  var config = {
    "nodePath" : process.execPath,
    "cliScript" : __filename,
    "activated" : activated
  }

  fs.writeFile(directory + "/config.json", JSON.stringify(config, null, 4), function(err) {
      if(err) {
        console.log(err);
      } else {
        //console.log("config saved to " + directory + "/config.json");
      }
  });

}

program
  .version('0.0.1')

// activation command
program
  .command('activate')
  .description('activate')
  .action(function(folder){

    prompt.start();

    prompt.get([ { description: 'Please confirm EULA agreement (Y)es, (N)o, or (V)iew', default: "Y", name: "eulaconfirm" }], function (err, result) {
        var eulaconfirm = result.eulaconfirm.toLowerCase();
        if (eulaconfirm == 'y') {
          prompt.get([ { description: 'Please enter Product Key or (G)et free key: ', name: "productkey" }], function (err, result) {
            var productkey = result.productkey.toLowerCase();
            if (productkey == 'g') {
              console.log ("Opening Atomic Store in default browser window");
              open("https://store.atomicgameengine.com/site");
            } else {
              cli.activate(productkey)
              .then(function () {
                saveAtomicConfig(true);
              })
            }
          });
        } else if (eulaconfirm == 'v') {
          console.log ("Opening EULA in default browser window");
          open("https://github.com/AtomicGameEngine/AtomicGameEngine/blob/master/LICENSE.md");
        }
    });
  });

// deactivation
program
  .command('deactivate')
  .description('deactivates and returns a product activation to the server')
  .action(function(){
    cli.deactivate().
    then(function() {
      saveAtomicConfig(false);
    })
  });


// new project command
program
  .command('new <folder>')
  .description('creates a new project in the specified folder')
  .action(function(folder){
    cli.newProject(folder)
    .then(function () {
        console.log("New Atomic project created in " + path.resolve(folder));
    })
    .catch(function (error) {
        console.error("Error: Could not create " + path.resolve(folder));
        process.exit(1);
    });

  });

program
  .command('add <platform>')
  .description('adds a platform to the project')
  .action(function(platform){
    cli.addPlatform(platform)
    .then(function () {
    })
    .catch(function (error) {
        process.exit(1);
    });

  });


program
  .command('run <platform>')
  .option('--project <path>')
  .description('runs the project on a specified platform')
  .action(function(platform, options) {
    if(options.project) {
      process.chdir(options.project)
    }
    cli.run(platform)
    .then(function () {
    })
    .catch(function (error) {
        process.exit(1);
    });

  });


  program
    .command('build <platform>')
    .option('--project <path>')
    .description('builds the project for the specified platform')
    .action(function(platform, options) {
      if(options.project) {
        process.chdir(options.project)
      }
      cli.build(platform)
      .then(function () {
      })
      .catch(function (error) {
          process.exit(1);
      });

    });


program
  .command('edit [path_to_project]')
  .description('edits the project in the cwd or on at a specified path')
  .action(function(path_to_project, options){

    var path = path_to_project || process.cwd();
    cli.atomiceditor(["-project", path], {output:true});
  });

// http server
program
  .command('serve')
  .option('-p, --port [8000]', 'The port to run the server on [8000]', '8000')
  .description('start a http server on the specified port which serves the current folder')
  .action(function(options) {
    var args = {}
    args.port = options.port;
    var server = require("../lib/httpserver");
    server.run(args)
  });


  program.parse(process.argv);

  if (!program.args.length) program.help();
