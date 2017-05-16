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

    /** Base for all event types */
    type EventType = string;

    /** Base for all event callback data */
    type EventData = Object;

    /** Base interface for events, contains eventType string and callback */
    interface EventMetaData {
        /**@internal*/
        _eventType?: string;
        /**@internal*/
        _callback?: (...params) => any;
    }

    /** Base interface for event data sent to event handlers */
    interface EventCallbackMetaData {
        /**@internal*/
        _eventType?: string;
        /**@internal*/
        _callbackData?: any;
    }

    interface NativeEvent extends EventMetaData { }

    interface ScriptEvent extends EventMetaData { }

    // typed callback generic
    type EventCallback<T extends EventMetaData> = (data: T) => void;

    /**
     * Utility function to wrap up an event callback to pass to subscribeToEvent
     * @param eventType The type of event to wrap
     * @param callback A callback to call when the event is fired
     */
    export function ScriptEvent<T extends Atomic.EventMetaData>(eventType: string, callback: Atomic.EventCallback<T>): Atomic.EventMetaData;

    /**
     * Utility function to wrap up event data to pass to sendEvent
     * @param eventType The type of event to wrap
     * @param callbackData The data to pass to the event subscriber
     */
    export function ScriptEventData<T extends Atomic.EventData>(eventType: string, callbackData?: T): Atomic.EventCallbackMetaData;

    export interface PathInfo {

        pathName: string;
        fileName: string;
        ext: string;

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

    export interface AttributeInfo {

        type: VariantType;
        name: string;
        mode: number; // AM_*
        defaultValue: string;
        enumNames: string[];
        enumValues: number[];
        resourceTypeName: string;
        dynamic: boolean;
        tooltip: string;
        isArray:boolean;
        fixedArraySize:number;
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


declare module ToolCore {

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
