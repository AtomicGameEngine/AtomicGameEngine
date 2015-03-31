

// CDN
var cdnroot = JSON.parse(fs.readFileSync(__dirname + "/../data/cdn.json"))
var branch;

// TODO: otherwise we need to use the package data
if (process.env.ATOMIC_DEV_LOCAL) {
  branch = "local";
}

var cdn = cdnroot[branch]
var cdnServer = cdn["server"];
var cdnEditorVersion = cdn["editor"]["version"];
var cdnAtomicToolVersion = cdn["atomictool"]["version"];

var home = osenv.home();
var atomicRootDir = home + "/.atomicgameengine/" + branch;

// CDN url for editor
var cdnEditorURL;

if (process.platform == "darwin") {
  cdnEditorURL = cdnServer + cdn["editor"] + "-" + cdnEditorVersion + ".zip";
}
