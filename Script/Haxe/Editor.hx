package atomic;


   // enum EditMode
typedef EditMode = Int;

   // enum AxisMode
typedef AxisMode = Int;
//Atomic Haxe Definitions

extern class Editor {


   // enum EditMode
    public static var EDIT_SELECT:EditMode;
    public static var EDIT_MOVE:EditMode;
    public static var EDIT_ROTATE:EditMode;
    public static var EDIT_SCALE:EditMode;

   // enum AxisMode
    public static var AXIS_WORLD:AxisMode;
    public static var AXIS_LOCAL:AxisMode;

   public static var FINDTEXT_FLAG_NONE: Int;
   public static var FINDTEXT_FLAG_CASESENSITIVE: Int;
   public static var FINDTEXT_FLAG_WHOLEWORD: Int;
   public static var FINDTEXT_FLAG_WRAP: Int;
   public static var FINDTEXT_FLAG_NEXT: Int;
   public static var FINDTEXT_FLAG_PREV: Int;
   public static var EDITOR_MODALERROR: Int;
   public static var EDITOR_MODALINFO: Int;


}

//----------------------------------------------------
// MODULE: Editor
//----------------------------------------------------


@:native("Atomic.FileUtils")
extern class FileUtils extends AObject {

      // Construct.
    function new();

    function createDirs(folder: String): Bool;
    function openProjectFileDialog(): String;
    function newProjectFileDialog(): String;
    function revealInFinder(fullpath: String): Void;

}

@:native("Atomic.EditorMode")
extern class EditorMode extends AObject {

      // Construct.
    function new();

    function playProject(): Bool;

}

@:native("Atomic.PlayerMode")
extern class PlayerMode extends AObject {

      // Construct.
    function new();

    function launchedByEditor(): Bool;

}

@:native("Atomic.JSResourceEditor")
extern class JSResourceEditor extends ResourceEditor {

    function new(fullpath: String, container: Atomic.UITabContainer);

    @:overload(function(findText: String, flags: Int): Bool{})
    override function findText(text: String, flags: Int): Bool;
    @:overload(function(): Void{})
    override function findTextClose(): Void;
    function gotoTokenPos(tokenPos: Int): Void;
    function gotoLineNumber(lineNumber: Int): Void;
    function formatCode(): Void;
    @:overload(function(): Void{})
    override function setFocus(): Void;
    @:overload(function(): Bool{})
    override function hasUnsavedModifications(): Bool;
    @:overload(function(): Bool{})
    override function save(): Bool;

}

@:native("Atomic.ResourceEditor")
extern class ResourceEditor extends AObject {

    var button: Atomic.UIButton;
    var fullPath: String;
    var rootContentWidget: Atomic.UIWidget;

    function new(fullpath: String, container: Atomic.UITabContainer);

    function getButton(): Atomic.UIButton;
    function hasUnsavedModifications(): Bool;
    function setFocus(): Void;
    function close(?navigateToAvailableResource: Bool): Void;
    function findText(text: String, flags: Int): Bool;
    function findTextClose(): Void;
    function requiresInspector(): Bool;
    function getFullPath(): String;
    function save(): Bool;
    function getRootContentWidget(): Atomic.UIWidget;

}

@:native("Atomic.SceneEditor3D")
extern class SceneEditor3D extends ResourceEditor {

    var scene: Atomic.Scene;

    function new(fullpath: String, container: Atomic.UITabContainer);

    function selectNode(node: Atomic.Node): Void;
    function getScene(): Atomic.Scene;
    @:overload(function(): Void{})
    override function setFocus(): Void;
    @:overload(function(): Bool{})
    override function requiresInspector(): Bool;
    @:overload(function(?navigateToAvailableResource: Bool): Void{})
    override function close(?navigateToAvailableResource: Bool): Void;
    @:overload(function(): Bool{})
    override function save(): Bool;

}

@:native("Atomic.SceneView3D")
extern class SceneView3D extends UISceneView {

    var pitch: Float;
    var yaw: Float;

    function new(sceneEditor: SceneEditor3D);

    function selectNode(node: Atomic.Node): Void;
    function setPitch(pitch: Float): Void;
    function setYaw(yaw: Float): Void;
    function enable(): Void;
    function disable(): Void;
    function isEnabled(): Bool;

}


