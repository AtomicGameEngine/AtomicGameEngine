# atomic-cli

Command line tools for the [Atomic Game Engine](http://www.atomicgameengine.com)

##### Website

<a href="http://atomicgameengine.com">http://www.AtomicGameEngine.com</a>

##### Chat

[![Join the chat at https://gitter.im/AtomicGameEngine/AtomicGameEngine](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/AtomicGameEngine/AtomicGameEngine?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

##### Forums

<a href="http://atomicgameengine.com/forum">Atomic Game Engine Forums</a>

##### Twitter

<a href="https://twitter.com/AtomicGEngine">Follow @AtomicGEngine</a>

[WelcomeScreen]: https://github.com/AtomicGameEngine/AtomicExamples/wiki/images/WelcomeScreen.png

![alt text][WelcomeScreen]

# Install

atomic-cli must be installed and activated

```
npm install atomic-cli -g
atomic-cli activate
```

# usage

```
Usage: atomic-cli

  Commands:

    activate                  activate
    deactivate                deactivates
    new <folder>              creates project in the specified folder
    add <platform>            adds a platform to the project
                              (mac|windows|ios|android|web)
    run [options] <platform>  builds and runs the project on a specified platform
    edit [path_to_project]    opens the Atomic Editor loading specified project

  Options:

    -h, --help     output usage information
    -V, --version  output the version number
```
