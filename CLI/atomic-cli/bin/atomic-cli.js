#!/usr/bin/env node
"use strict";

// https://github.com/yeoman/update-notifier

// https://github.com/tj/commander.js
var path = require("path");
var program = require('commander');
var cli = require("atomic-cli")
var open = require("open");
var prompt = require('prompt');

prompt.message = "";
prompt.delimiter = "";

program
  .version('0.0.1')
  .parse(process.argv);

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
              cli.activate(productkey);
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
    cli.deactivate();
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
  .command('run <platform> [no-build]')
  .description('runs the project on a specified platform')
  .action(function(platform){
    cli.run(platform)
    .then(function () {
    })
    .catch(function (error) {
        process.exit(1);
    });

  });

program
  .command('edit [path-to-project]')
  .description('edits the project in the cwd or on at a specified path')
  .action(function(options){
    cli.atomiceditor(["-project", process.cwd()], {output:true});
  });

  program.parse(process.argv);

  if (!program.args.length) program.help();
