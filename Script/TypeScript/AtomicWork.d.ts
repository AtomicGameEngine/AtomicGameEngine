/// <reference path="Atomic.d.ts" />
/// <reference path="ToolCore.d.ts" />
/// <reference path="Editor.d.ts" />
/// <reference path="AtomicPlayer.d.ts" />

declare module Atomic {

    export function print(...args: any[]);

    export var platform: string;

    // subsystems

    export var engine: Engine;
    export var graphics: Graphics;
    export var renderer: Renderer;
    export var cache: ResourceCache;
    export var input: Input;
    export var fileSystem: FileSystem;
    export var network: Network;
    export var ui: UI;
    export var audio: Audio;
    export var player: AtomicPlayer.Player;

    export var editorMode: Editor.EditorMode;

    // end subsystems

    // Base interface for events
    export interface NativeEvent { }

    export interface PathInfo {

        pathName: string;
        fileName: string;
        ext: string;

    }

    export interface ScreenModeEvent extends Atomic.NativeEvent {

        width: number;
        height: number;
        fullscreen: boolean;
        resizable: boolean;
        borderless: boolean;

    }

    export interface Ray {
        /** Ray origin */
        origin: Atomic.Vector3;

        /** Ray direction */
        direction: Atomic.Vector3;
    }

    export interface Camera {
        getScreenRay(x: number, y: number): Atomic.Ray;
    }

    export interface Octree {
        /**
         * Cast a ray returing a single hit
         * @param  {Atomic.Ray} ray
         * @param  {Atomic.RayQueryLevel} level defaults to Atomic.RAY_TRIANGLE
         * @param  {number} maxDistance defaults to Atomic.M_INFINITY
         * @param  {number} drawableFlags defaults to Atomic.DRAWABLE_ANY
         * @param  {number} viewMask defaults to Atomic.DEFAULT_VIEWMASK
         * @return {Atomic.RayQueryResult}
         */
        rayCastSingle(ray: Atomic.Ray, level?: Atomic.RayQueryLevel, maxDistance?: number, drawableFlags?: number, viewMask?: number):Atomic.RayQueryResult;

        /**
         * Cast a ray returning all hits
         * @param  {Atomic.Ray} ray
         * @param  {Atomic.RayQueryLevel} level defaults to Atomic.RAY_TRIANGLE
         * @param  {number} maxDistance defaults to Atomic.M_INFINITY
         * @param  {number} drawableFlags defaults to Atomic.DRAWABLE_ANY
         * @param  {number} viewMask defaults to Atomic.DEFAULT_VIEWMASK
         * @return {Atomic.RayQueryResult}
         */
        rayCast(ray: Atomic.Ray, level?: Atomic.RayQueryLevel, maxDistance?: number, drawableFlags?: number, viewMask?: number):Atomic.RayQueryResult[];
    }

    export interface RayQueryResult {
        /** Hit position in world space. */
        position: Atomic.Vector3;

        /** Hit normal in world space. Negation of ray direction if per-triangle data not available. */
        normal: Atomic.Vector3;

        /** Hit texture position */
        textureUV: Atomic.Vector2;

        /** Distance from ray origin. */
        distance:number;

        /** Drawable. */
        drawable: any;

        /** Scene node. */
        node: Atomic.Node;

        /** Drawable specific subobject if applicable. */
        subObject: number;
    }

    export interface KeyDownEvent extends NativeEvent {

        // keycode
        key: number;
        //  Atomic.QUAL_SHIFT, Atomic.QUAL_CTRL, Atomic.QUAL_ALT, Atomic.QUAL_ANY
        qualifiers: number;

        // mouse buttons down
        buttons: number;

    }

    export interface KeyUpEvent extends NativeEvent {

        // keycode
        key: number;
        //  Atomic.QUAL_SHIFT, Atomic.QUAL_CTRL, Atomic.QUAL_ALT, Atomic.QUAL_ANY
        qualifiers: number;
        // mouse buttons down
        buttons: number;

    }

    export interface UIShortcutEvent extends NativeEvent {

        // keycode
        key: number;
        //  Atomic.QUAL_SHIFT, Atomic.QUAL_CTRL, Atomic.QUAL_ALT, Atomic.QUAL_ANY
        qualifiers: number;

    }

    export interface UIListViewSelectionChangedEvent extends NativeEvent {

        refid: string;
        selected: boolean;

    }

    export interface NodeAddedEvent extends NativeEvent {

        scene: Atomic.Scene;
        parent: Atomic.Node;
        node: Atomic.Node;

    }

    export interface NodeRemovedEvent extends NativeEvent {

        scene: Atomic.Scene;
        parent: Atomic.Node;
        node: Atomic.Node;

    }

    export interface NodeNameChangedEvent extends NativeEvent {

        scene: Atomic.Scene;
        node: Atomic.Node;

    }

    export interface UIWidgetEvent extends NativeEvent {

        handler: UIWidget;
        target: UIWidget;
        type: number; /*UIWidgetEventType*/
        x: number;
        y: number;
        deltax: number;
        deltay: number;
        count: number;
        key: number;
        specialkey: number;
        modifierkeys: number;
        refid: string;
        touch: boolean;
    }

    export interface UIWidgetFocusChangedEvent extends NativeEvent {
        widget: UIWidget;
        focused: boolean;
    }

    export interface UIWidgetEditCompleteEvent extends NativeEvent {
        widget: UIWidget;
    }

    export interface UIWidgetDeletedEvent extends NativeEvent {

        widget: UIWidget;
    }

    export interface DragBeginEvent extends NativeEvent {

        source: UIWidget;
        dragObject: UIDragObject;
    }

    export interface DragEnterWidgetEvent extends NativeEvent {

        widget: UIWidget;
        dragObject: UIDragObject;
    }

    export interface DragExitWidgetEvent extends NativeEvent {

        widget: UIWidget;
        dragObject: UIDragObject;
    }

    export interface DragEndedEvent extends NativeEvent {

        target: UIWidget;
        dragObject: UIDragObject;
    }

    export interface TemporaryChangedEvent extends NativeEvent {

        serializable: Atomic.Serializable;

    }

    export interface ComponentAddedEvent extends NativeEvent {

        scene: Atomic.Scene;
        node: Atomic.Node;
        component: Atomic.Component;

    }

    export interface ComponentRemovedEvent extends NativeEvent {

        scene: Atomic.Scene;
        node: Atomic.Node;
        component: Atomic.Component;

    }

    export interface IPCJSErrorEvent extends NativeEvent {

        errorName: string;
        errorMessage: string;
        errorFileName: string;
        errorLineNumber: number;
        errorStack: string;

    }


    export interface IPCMessageEvent extends NativeEvent {

        message: string;
        value: number;
    }

    export interface AttributeInfo {

        type: VariantType;
        name: string;
        mode: number; // AM_*
        defaultValue: string;
        enumNames: string[];
        resourceTypeName: string;
        dynamic: boolean;
        tooltip: string;

    }

    export interface ShaderParameter {

        name: string;
        value: any;
        valueString: string;
        typeName: string;
        type: VariantType;

    }

    export function getArguments(): Array<string>;
    export function getEngine(): Engine;
    export function getInput(): Input;
    export function getGraphics(): Graphics;
    export function getFileSystem(): FileSystem;
    export function getResourceCache(): ResourceCache;
    export function getRenderer(): Atomic.Renderer;
    export function getNetwork(): Atomic.Network;
    export function getUI(): Atomic.UI;

    export function assert();
    export function js_module_read_file(path: string);
    export function openConsoleWindow();
    export function script(script: string): boolean;
    export function destroy(node: Atomic.Node): boolean;
    export function destroy(scene: Atomic.Scene): boolean;
    export function destroy(component: Atomic.JSComponent): boolean;

    export function getParentPath(path: string): string;
    export function getPath(path: string): string;
    export function addTrailingSlash(path: string): string;
    export function getExtension(path: string): string;

    export function splitPath(path: string): PathInfo;

}

declare module Editor {

    export interface SceneNodeSelectedEvent extends Atomic.NativeEvent {
        scene: Atomic.Scene;
        node: Atomic.Node;
        selected: boolean;
        quiet: boolean;
    }

    export interface SceneEditAddRemoveNodesEvent extends Atomic.NativeEvent {

        end: boolean;

    }


    export interface SceneEditNodeAddedEvent extends Atomic.NativeEvent {

        scene: Atomic.Scene;
        parent: Atomic.Node;
        node: Atomic.Node;

    }

    export interface SceneEditNodeRemovedEvent extends Atomic.NativeEvent {

        scene: Atomic.Scene;
        parent: Atomic.Node;
        node: Atomic.Node;

    }

    export interface SceneEditComponentAddedRemovedEvent extends Atomic.NativeEvent {

        scene: Atomic.Scene;
        node: Atomic.Node;
        component: Atomic.Component;
        removed: boolean;
    }

    export interface SceneEditStateChangeEvent extends Atomic.NativeEvent {

        serializable: Atomic.Serializable;

    }

    export interface SceneEditNodeCreatedEvent extends Atomic.NativeEvent {
        node: Atomic.Node;
    }

    export interface GizmoEditModeChangedEvent extends Atomic.NativeEvent {
        mode: EditMode;
    }

    export interface GizmoAxisModeChangedEvent extends Atomic.NativeEvent {
        mode: AxisMode;
    }

}

declare module ToolCore {

    export interface ResourceAddedEvent extends Atomic.NativeEvent {

        guid: string;

    }

    export interface ResourceRemovedEvent extends Atomic.NativeEvent {

        guid: string;

    }

    export interface LicenseDeactivationErrorEvent extends Atomic.NativeEvent {

        message: string;

    }

    export interface AssetImportErrorEvent extends Atomic.NativeEvent {

        path: string;
        guid: string;
        error: string;
    }

    export interface AssetRenamedEvent extends Atomic.NativeEvent {

        asset: Asset;

    }

    export interface AssetMovedEvent extends Atomic.NativeEvent {

        asset: Asset;
        oldPath: string;

    }


    export interface PlatformChangedEvent extends Atomic.NativeEvent {

        platform: ToolCore.Platform;

    }

    export interface BuildOutputEvent extends Atomic.NativeEvent {

        text: string;

    }

    export interface BuildCompleteEvent extends Atomic.NativeEvent {

        platformID: number;
        message: string;
        success: boolean;
        buildFolder: string;

    }

    export interface NETBuildResult {

        success: boolean;
        build: NETBuild;
        errorText: string;
    }

    export var toolEnvironment: ToolEnvironment;
    export var toolSystem: ToolSystem;
    export var assetDatabase: AssetDatabase;
    export var licenseSystem: LicenseSystem;
    export var buildSystem: BuildSystem;
    export var netProjectSystem: NETProjectSystem;

    export function getToolEnvironment(): ToolEnvironment;
    export function getToolSystem(): ToolSystem;
    export function getAssetDatabase(): AssetDatabase;
    export function getLicenseSystem(): LicenseSystem;
}
