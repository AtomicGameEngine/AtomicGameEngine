AtomicCLI = require 'atomic-cli'
glob = require 'glob'
{CompositeDisposable} = require 'atom'

module.exports = AtomicGameEngine =
  subscriptions: null

  activate: (state) ->
    # Events subscribed to in atom's system can be easily cleaned up with a CompositeDisposable
    @subscriptions = new CompositeDisposable

    # Register command that toggles this view
    @subscriptions.add atom.commands.add 'atom-workspace', 'atomic-game-engine:openEditor': => @openEditor()
    @subscriptions.add atom.commands.add 'atom-workspace', 'atomic-game-engine:run': => @run()

  deactivate: ->
    @subscriptions.dispose()

  serialize: ->


  getAtomicProjectPath: ->

    paths = atom.project.getPaths();

    for path in paths
      files = glob.sync path + "/*.atomic"
      if files.length
        return path

    return null

  openEditor: ->

    path = @getAtomicProjectPath()

    if path
      AtomicCLI.atomiceditor ["-project", path]

  run: ->

    path = @getAtomicProjectPath()

    if path
      process.chdir(path)
      AtomicCLI.run("mac")
