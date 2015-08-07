package;


   // enum PlatformID
typedef PlatformID = {
    var PLATFORMID_UNDEFINED:Int;
    var PLATFORMID_WINDOWS:Int;
    var PLATFORMID_MAC:Int;
    var PLATFORMID_ANDROID:Int;
    var PLATFORMID_IOS:Int;
    var PLATFORMID_WEB:Int;
}
//Atomic Haxe Definitions

extern class ToolCore {


   public static var PROJECTFILE_VERSION: Int;


}

//----------------------------------------------------
// MODULE: ToolCore
//----------------------------------------------------


@:native("Atomic.ToolEnvironment")
extern class ToolEnvironment extends AObject {

    var rootSourceDir: String;
    var rootBuildDir: String;
    var editorBinary: String;
    var playerBinary: String;
    var toolBinary: String;
    var coreDataDir: String;
    var playerDataDir: String;
    var editorDataDir: String;
    var deploymentDataDir: String;
    var projectTemplatesDir: String;
    var examplesDir: String;
    var devConfigFilename: String;

    function new();

    function initFromPackage(): Bool;
    function initFromJSON(?atomicTool: Bool): Bool;
      // Root source and build directories for development source tree builds
    function setRootSourceDir(sourceDir: String): Void;
    function setRootBuildDir(buildDir: String, ?setBinaryPaths: Bool): Void;
    function getRootSourceDir(): String;
    function getRootBuildDir(): String;
      // Binaries
    function getEditorBinary(): String;
    function getPlayerBinary(): String;
    function getToolBinary(): String;
      // Resource directories
    function getCoreDataDir(): String;
    function getPlayerDataDir(): String;
    function getEditorDataDir(): String;
      // Data directories
    function getDeploymentDataDir(): String;
    function getProjectTemplatesDir(): String;
    function getExamplesDir(): String;
    function getDevConfigFilename(): String;
    function dump(): Void;

}

@:native("Atomic.ToolSystem")
extern class ToolSystem extends AObject {

    var project: Project;
    var dataPath: String;
    var currentPlatform: Platform;

    function new();

    function loadProject(fullpath: String): Bool;
    function getProject(): Project;
    function closeProject(): Void;
    function getDataPath(): String;
    function setDataPath(path: String): Void;
    function registerPlatform(platform: Platform): Void;
    function getPlatformByID(platform: PlatformID): Platform;
    function getPlatformByName(name: String): Platform;
    function setCurrentPlatform(platform: PlatformID): Void;
    function getCurrentPlatform(): Platform;
    function setCLI(): Void;
    function isCLI(): Bool;

}

@:native("Atomic.Project")
extern class Project extends AObject {

    var resourcePath: String;
    var componentsPath: String;
    var scriptsPath: String;
    var modulesPath: String;
    var projectPath: String;
    var projectFilePath: String;
    var userPrefsFullPath: String;
    var buildSettingsFullPath: String;
    var version: String;

      // Construct.
    function new();

    function load(fullpath: String): Bool;
    function save(fullpath: String): Void;
      // Paths
    function getResourcePath(): String;
    function setResourcePath(resourcePath: String): Void;
    function getComponentsPath(): String;
    function getScriptsPath(): String;
    function getModulesPath(): String;
    function isComponentsDirOrFile(fullPath: String): Bool;
    function isScriptsDirOrFile(fullPath: String): Bool;
    function isModulesDirOrFile(fullPath: String): Bool;
    function addPlatform(platformID: PlatformID): Void;
    function containsPlatform(platformID: PlatformID): Bool;
    function removePlatform(platformID: PlatformID): Void;
    function isDirty(): Bool;
    function setDirty(): Void;
    function getProjectPath(): String;
    function getProjectFilePath(): String;
    function getUserPrefsFullPath(): String;
    function getBuildSettingsFullPath(): String;
    function getVersion(): String;
    function setVersion(version: String): Void;
    function saveBuildSettings(): Void;
    function loadBuildSettings(): Bool;
    function saveUserPrefs(): Void;
    function loadUserPrefs(): Bool;

}

@:native("Atomic.ProjectFile")
extern class ProjectFile extends AObject {

      // Construct.
    function new();

    function save(project: Project): Void;
    function load(project: Project): Bool;
    function writeNewProject(fullpath: String): Void;

}

@:native("Atomic.Platform")
extern class Platform extends AObject {

    var name: String;
    var platformID: PlatformID;

    function new();

    function getName(): String;
    function getPlatformID(): PlatformID;

}

@:native("Atomic.PlatformMac")
extern class PlatformMac extends Platform {


    function new();

    @:overload(function(): String{})
    override function getName(): String;
    @:overload(function(): PlatformID{})
    override function getPlatformID(): PlatformID;

}

@:native("Atomic.PlatformWeb")
extern class PlatformWeb extends Platform {


    function new();

    @:overload(function(): String{})
    override function getName(): String;
    @:overload(function(): PlatformID{})
    override function getPlatformID(): PlatformID;

}

@:native("Atomic.PlatformWindows")
extern class PlatformWindows extends Platform {


    function new();

    @:overload(function(): String{})
    override function getName(): String;
    @:overload(function(): PlatformID{})
    override function getPlatformID(): PlatformID;

}

@:native("Atomic.Command")
extern class Command extends AObject {

    function new();

    function parse(command: String): Bool;
    function run(): Void;
    function finished(): Void;
    function error(errorMsg: String): Void;
    function cancel(): Void;
    function requiresProjectLoad(): Bool;
    function requiresLicenseValidation(): Bool;

}

@:native("Atomic.PlayCmd")
extern class PlayCmd extends Command {

    function new();

    @:overload(function(): Void{})
    override function run(): Void;

}

@:native("Atomic.OpenAssetImporter")
extern class OpenAssetImporter extends AObject {

    var errorMessage: String;
    var importNode: Atomic.Node;
    var startTime: Float;
    var endTime: Float;
    var scale: Float;
    var exportAnimations: Bool;
    var verboseLog: Bool;

    function new();

    function load(assetPath: String): Bool;
    function getErrorMessage(): String;
    function exportModel(outName: String, ?animName: String, ?animationOnly: Bool): Bool;
    function setImportNode(node: Atomic.Node): Void;
    function setStartTime(startTime: Float): Void;
    function setEndTime(endTime: Float): Void;
    function setScale(scale: Float): Void;
    function setExportAnimations(exportAnimations: Bool): Void;
    function setVerboseLog(verboseLog: Bool): Void;

}

@:native("Atomic.Asset")
extern class Asset extends AObject {

    var guid: String;
    var name: String;
    var path: String;
    var relativePath: String;
    var cachePath: String;
    var importerType: String;
    var importerTypeName: String;
    var importer: AssetImporter;
    var parent: Asset;
    var dirty: Bool;
    var fileTimestamp: Int;
    var dotAssetFilename: String;

      // Construct.
    function new();

    function import(): Bool;
    function preload(): Bool;
    function setPath(path: String): Bool;
    function getGUID(): String;
    function getName(): String;
    function getPath(): String;
      // Get the path relative to project
    function getRelativePath(): String;
    function getCachePath(): String;
    function getImporterType(): String;
    function getImporterTypeName(): String;
    function getImporter(): AssetImporter;
    function postImportError(message: String): Void;
    function getParent(): Asset;
    function setDirty(dirty: Bool): Void;
    function isDirty(): Bool;
      // Get the last timestamp as seen by the AssetDatabase
    function getFileTimestamp(): Int;
      // Sets the time stamp to the asset files current time
    function updateFileTimestamp(): Void;
    function getDotAssetFilename(): String;
    function isFolder(): Bool;
    function load(): Bool;
    function save(): Bool;

}

@:native("Atomic.AssetDatabase")
extern class AssetDatabase extends AObject {

    var cachePath: String;

      // Construct.
    function new();

    function getAssetByGUID(guid: String): Asset;
    function getAssetByPath(path: String): Asset;
    function getAssetByCachePath(cachePath: String): Asset;
    function generateAssetGUID(): String;
    function registerGUID(guid: String): Void;
    function getCachePath(): String;
    function deleteAsset(asset: Asset): Void;
    function scan(): Void;
    function getDotAssetFilename(path: String): String;

}

@:native("Atomic.AssetImporter")
extern class AssetImporter extends AObject {

    var asset: Asset;

      // Construct.
    function new(asset: Asset);

    function setDefaults(): Void;
    function preload(): Bool;
    function getAsset(): Asset;
    function requiresCacheFile(): Bool;

}

@:native("Atomic.AudioImporter")
extern class AudioImporter extends AssetImporter {

      // Construct.
    function new(asset: Asset);

    @:overload(function(): Void{})
    override function setDefaults(): Void;

}

@:native("Atomic.JavascriptImporter")
extern class JavascriptImporter extends AssetImporter {

      // Construct.
    function new(asset: Asset);

    @:overload(function(): Void{})
    override function setDefaults(): Void;
    function isComponentFile(): Bool;

}

@:native("Atomic.MaterialImporter")
extern class MaterialImporter extends AssetImporter {

      // Construct.
    function new(asset: Asset);

    @:overload(function(): Void{})
    override function setDefaults(): Void;
    function saveMaterial(): Void;

}

@:native("Atomic.AnimationImportInfo")
extern class AnimationImportInfo extends AObject {

    var name: String;
    var startTime: Float;
    var endTime: Float;

    function new();

    function getName(): String;
    function getStartTime(): Float;
    function getEndTime(): Float;
    function setName(name: String): Void;
    function setStartTime(time: Float): Void;
    function setEndTime(time: Float): Void;

}

@:native("Atomic.ModelImporter")
extern class ModelImporter extends AssetImporter {

    var scale: Float;
    var importAnimations: Bool;
    var animationCount: Int;

      // Construct.
    function new(asset: Asset);

    @:overload(function(): Void{})
    override function setDefaults(): Void;
    function getScale(): Float;
    function setScale(scale: Float): Void;
    function getImportAnimations(): Bool;
    function setImportAnimations(importAnimations: Bool): Void;
    function getAnimationCount(): Int;
    function setAnimationCount(count: Int): Void;
    function getAnimationInfo(index: Int): AnimationImportInfo;

}

@:native("Atomic.PrefabImporter")
extern class PrefabImporter extends AssetImporter {

      // Construct.
    function new(asset: Asset);

    @:overload(function(): Void{})
    override function setDefaults(): Void;
    @:overload(function(): Bool{})
    override function preload(): Bool;

}

@:native("Atomic.TextureImporter")
extern class TextureImporter extends AssetImporter {

      // Construct.
    function new(asset: Asset);

    @:overload(function(): Void{})
    override function setDefaults(): Void;

}

@:native("Atomic.LicenseSystem")
extern class LicenseSystem extends AObject {

    var key: String;
    var email: String;

      // Construct.
    function new();

    function initialize(): Void;
    function licenseWindows(): Bool;
    function licenseMac(): Bool;
    function licenseAndroid(): Bool;
    function licenseIOS(): Bool;
    function licenseHTML5(): Bool;
    function licenseModule3D(): Bool;
      // Returns whether there are any platform licenses available
    function isStandardLicense(): Bool;
      // Returns true if request to deactivate is made
    function deactivate(): Bool;
    function resetLicense(): Void;
    function loadLicense(): Bool;
      // Basic key validation
    function validateKey(key: String): Bool;
      // Activate on server
    function requestServerActivation(key: String): Void;
    function getKey(): String;
    function generateMachineID(): String;
    function getEmail(): String;
    function licenseAgreementConfirmed(): Void;

}


