#!/usr/bin/env node
"use strict";

// https://github.com/yeoman/update-notifier

// https://github.com/tj/commander.js
var path = require("path");
var program = require('commander');
var cli = require("atomic-cli")



program
  .version('0.0.1')
  .parse(process.argv);

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
