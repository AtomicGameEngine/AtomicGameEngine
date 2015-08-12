//Atomic TypeScript Definitions


/// <reference path="Atomic.d.ts" />

declare module ToolCore {


   // enum PlatformID
   export type PlatformID = number;
   export var PLATFORMID_UNDEFINED: PlatformID;
   export var PLATFORMID_WINDOWS: PlatformID;
   export var PLATFORMID_MAC: PlatformID;
   export var PLATFORMID_ANDROID: PlatformID;
   export var PLATFORMID_IOS: PlatformID;
   export var PLATFORMID_WEB: PlatformID;


   export var PROJECTFILE_VERSION: number;


//----------------------------------------------------
// MODULE: ToolCore
//----------------------------------------------------


   export class ToolEnvironment extends Atomic.AObject {

      rootSourceDir: string;
      rootBuildDir: string;
      editorBinary: string;
      playerBinary: string;
      toolBinary: string;
      coreDataDir: string;
      playerDataDir: string;
      editorDataDir: string;
      deploymentDataDir: string;
      projectTemplatesDir: string;
      examplesDir: string;
      devConfigFilename: string;

      constructor();

      initFromPackage(): boolean;
      initFromJSON(atomicTool?: boolean): boolean;
      // Root source and build directories for development source tree builds
      setRootSourceDir(sourceDir: string): void;
      setRootBuildDir(buildDir: string, setBinaryPaths?: boolean): void;
      getRootSourceDir(): string;
      getRootBuildDir(): string;
      // Binaries
      getEditorBinary(): string;
      getPlayerBinary(): string;
      getToolBinary(): string;
      // Resource directories
      getCoreDataDir(): string;
      getPlayerDataDir(): string;
      getEditorDataDir(): string;
      // Data directories
      getDeploymentDataDir(): string;
      getProjectTemplatesDir(): string;
      getExamplesDir(): string;
      getDevConfigFilename(): string;
      dump(): void;

   }

   export class ToolSystem extends Atomic.AObject {

      project: Project;
      dataPath: string;
      currentPlatform: Platform;

      constructor();

      loadProject(fullpath: string): boolean;
      getProject(): Project;
      closeProject(): void;
      getDataPath(): string;
      setDataPath(path: string): void;
      registerPlatform(platform: Platform): void;
      getPlatformByID(platform: PlatformID): Platform;
      getPlatformByName(name: string): Platform;
      setCurrentPlatform(platform: PlatformID): void;
      getCurrentPlatform(): Platform;
      setCLI(): void;
      isCLI(): boolean;

   }

   export class ProjectFile extends Atomic.AObject {

      // Construct.
      constructor();

      save(project: Project): void;
      load(project: Project): boolean;
      writeNewProject(fullpath: string): void;

   }

   export class Project extends Atomic.AObject {

      resourcePath: string;
      componentsPath: string;
      scriptsPath: string;
      modulesPath: string;
      projectPath: string;
      projectFilePath: string;
      userPrefsFullPath: string;
      buildSettingsFullPath: string;
      version: string;

      // Construct.
      constructor();

      load(fullpath: string): boolean;
      save(fullpath: string): void;
      // Paths
      getResourcePath(): string;
      setResourcePath(resourcePath: string): void;
      getComponentsPath(): string;
      getScriptsPath(): string;
      getModulesPath(): string;
      isComponentsDirOrFile(fullPath: string): boolean;
      isScriptsDirOrFile(fullPath: string): boolean;
      isModulesDirOrFile(fullPath: string): boolean;
      addPlatform(platformID: PlatformID): void;
      containsPlatform(platformID: PlatformID): boolean;
      removePlatform(platformID: PlatformID): void;
      isDirty(): boolean;
      setDirty(): void;
      getProjectPath(): string;
      getProjectFilePath(): string;
      getUserPrefsFullPath(): string;
      getBuildSettingsFullPath(): string;
      getVersion(): string;
      setVersion(version: string): void;
      saveBuildSettings(): void;
      loadBuildSettings(): boolean;
      saveUserPrefs(): void;
      loadUserPrefs(): boolean;

   }

   export class Platform extends Atomic.AObject {

      name: string;
      platformID: PlatformID;

      constructor();

      getName(): string;
      getPlatformID(): PlatformID;

   }

   export class PlatformMac extends Platform {

      name: string;
      platformID: PlatformID;

      constructor();

      getName(): string;
      getPlatformID(): PlatformID;

   }

   export class PlatformWeb extends Platform {

      name: string;
      platformID: PlatformID;

      constructor();

      getName(): string;
      getPlatformID(): PlatformID;

   }

   export class PlatformWindows extends Platform {

      name: string;
      platformID: PlatformID;

      constructor();

      getName(): string;
      getPlatformID(): PlatformID;

   }

   export class Command extends Atomic.AObject {

      constructor();

      parse(command: string): boolean;
      run(): void;
      finished(): void;
      error(errorMsg: string): void;
      cancel(): void;
      requiresProjectLoad(): boolean;
      requiresLicenseValidation(): boolean;

   }

   export class PlayCmd extends Command {

      constructor();

      run(): void;

   }

   export class OpenAssetImporter extends Atomic.AObject {

      errorMessage: string;
      importNode: Atomic.Node;
      startTime: number;
      endTime: number;
      scale: number;
      exportAnimations: boolean;
      verboseLog: boolean;

      constructor();

      load(assetPath: string): boolean;
      getErrorMessage(): string;
      exportModel(outName: string, animName?: string, animationOnly?: boolean): boolean;
      setImportNode(node: Atomic.Node): void;
      setStartTime(startTime: number): void;
      setEndTime(endTime: number): void;
      setScale(scale: number): void;
      setExportAnimations(exportAnimations: boolean): void;
      setVerboseLog(verboseLog: boolean): void;

   }

   export class AudioImporter extends AssetImporter {

      // Construct.
      constructor(asset: Asset);

      setDefaults(): void;

   }

   export class MaterialImporter extends AssetImporter {

      // Construct.
      constructor(asset: Asset);

      setDefaults(): void;
      saveMaterial(): void;

   }

   export class PrefabImporter extends AssetImporter {

      // Construct.
      constructor(asset: Asset);

      setDefaults(): void;
      preload(): boolean;

   }

   export class TextureImporter extends AssetImporter {

      // Construct.
      constructor(asset: Asset);

      setDefaults(): void;

   }

   export class Asset extends Atomic.AObject {

      guid: string;
      name: string;
      path: string;
      extension: string;
      relativePath: string;
      cachePath: string;
      resource: Atomic.Resource;
      importerType: string;
      importerTypeName: string;
      importer: AssetImporter;
      parent: Asset;
      dirty: boolean;
      fileTimestamp: number;
      dotAssetFilename: string;

      // Construct.
      constructor();

      import(): boolean;
      preload(): boolean;
      setPath(path: string): boolean;
      getGUID(): string;
      getName(): string;
      getPath(): string;
      getExtension(): string;
      // Get the path relative to project
      getRelativePath(): string;
      getCachePath(): string;
      getResource(): Atomic.Resource;
      getImporterType(): string;
      getImporterTypeName(): string;
      getImporter(): AssetImporter;
      postImportError(message: string): void;
      getParent(): Asset;
      setDirty(dirty: boolean): void;
      isDirty(): boolean;
      // Get the last timestamp as seen by the AssetDatabase
      getFileTimestamp(): number;
      // Sets the time stamp to the asset files current time
      updateFileTimestamp(): void;
      getDotAssetFilename(): string;
      isFolder(): boolean;
      load(): boolean;
      save(): boolean;

   }

   export class AssetDatabase extends Atomic.AObject {

      cachePath: string;

      // Construct.
      constructor();

      getAssetByGUID(guid: string): Asset;
      getAssetByPath(path: string): Asset;
      getAssetByCachePath(cachePath: string): Asset;
      generateAssetGUID(): string;
      registerGUID(guid: string): void;
      getCachePath(): string;
      deleteAsset(asset: Asset): void;
      scan(): void;
      getResourceImporterName(resourceTypeName: string): string;
      getDotAssetFilename(path: string): string;
      getFolderAssets(folder:string):ToolCore.Asset[];
      getAssetsByImporterType(type:string):ToolCore.Asset[];

   }

   export class AssetImporter extends Atomic.AObject {

      asset: Asset;
      resource: Atomic.Resource;

      // Construct.
      constructor(asset: Asset);

      setDefaults(): void;
      preload(): boolean;
      getAsset(): Asset;
      getResource(): Atomic.Resource;
      requiresCacheFile(): boolean;

   }

   export class JavascriptImporter extends AssetImporter {

      resource: Atomic.Resource;

      // Construct.
      constructor(asset: Asset);

      setDefaults(): void;
      isComponentFile(): boolean;
      getResource(): Atomic.Resource;

   }

   export class AnimationImportInfo extends Atomic.AObject {

      name: string;
      startTime: number;
      endTime: number;

      constructor();

      getName(): string;
      getStartTime(): number;
      getEndTime(): number;
      setName(name: string): void;
      setStartTime(time: number): void;
      setEndTime(time: number): void;

   }

   export class ModelImporter extends AssetImporter {

      scale: number;
      importAnimations: boolean;
      animationCount: number;
      resource: Atomic.Resource;

      // Construct.
      constructor(asset: Asset);

      setDefaults(): void;
      getScale(): number;
      setScale(scale: number): void;
      getImportAnimations(): boolean;
      setImportAnimations(importAnimations: boolean): void;
      getAnimationCount(): number;
      setAnimationCount(count: number): void;
      getResource(): Atomic.Resource;
      getAnimationInfo(index: number): AnimationImportInfo;

   }

   export class LicenseSystem extends Atomic.AObject {

      key: string;
      email: string;

      // Construct.
      constructor();

      initialize(): void;
      licenseWindows(): boolean;
      licenseMac(): boolean;
      licenseAndroid(): boolean;
      licenseIOS(): boolean;
      licenseHTML5(): boolean;
      licenseModule3D(): boolean;
      // Returns whether there are any platform licenses available
      isStandardLicense(): boolean;
      // Returns true if request to deactivate is made
      deactivate(): boolean;
      resetLicense(): void;
      loadLicense(): boolean;
      // Basic key validation
      validateKey(key: string): boolean;
      // Activate on server
      requestServerActivation(key: string): void;
      getKey(): string;
      generateMachineID(): string;
      getEmail(): string;
      licenseAgreementConfirmed(): void;

   }



}
