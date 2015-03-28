AtomicCLI = require 'atomic-cli'
glob = require 'glob'
{CompositeDisposable} = require 'atom'

module.exports = AtomicGameEngine =
  subscriptions: null

  activate: (state) ->
    # Events subscribed to in atom's system can be easily cleaned up with a CompositeDisposable
    @subscriptions = new CompositeDisposable

    # Register command that toggles this view
    @subscriptions.add atom.commands.add 'atom-workspace', 'atomic-game-engine:toggle': => @toggle()

  deactivate: ->
    @subscriptions.dispose()

  serialize: ->

  toggle: ->

    # AtomicCLI.atomiceditor([])
    paths = atom.project.getPaths();

    gotone = false;
    for path in paths
      glob path + "/*.atomic", (er, files) ->
        if files.length
          gotone = true
          AtomicCLI.atomiceditor ["-project", path]
      if gotone
        break

    console.log 'AtomicGameEngine was toggled!'
