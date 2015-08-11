/// <reference path="Atomic.d.ts" />
/// <reference path="ToolCore.d.ts" />
/// <reference path="Editor.d.ts" />


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

    export var editorMode: Editor.EditorMode;

    // end subsystems


    export interface PathInfo {

        pathName: string;
        fileName: string;
        ext: string;

    }

    export interface ScreenModeEvent {

        width: number;
        height: number;
        fullscreen: boolean;
        resizable: boolean;
        borderless: boolean;

    }


    export interface UIShortcutEvent {

        // keycode
        key: number;
        //  Atomic.QUAL_SHIFT, Atomic.QUAL_CTRL, Atomic.QUAL_ALT, Atomic.QUAL_ANY
        qualifiers: number;

    }

    export interface NodeAddedEvent {

        scene: Atomic.Scene;
        parent: Atomic.Node;
        node: Atomic.Node;

    }

    export interface NodeRemovedEvent {

        scene: Atomic.Scene;
        parent: Atomic.Node;
        node: Atomic.Node;

    }

    export interface NodeNameChangedEvent {

        scene: Atomic.Scene;
        node: Atomic.Node;

    }

    export interface UIWidgetEvent {

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

    export interface UIWidgetDeletedEvent {

        widget: UIWidget;
    }

    export interface DragBeginEvent {

        source: UIWidget;
        dragObject: UIDragObject;
    }

    export interface DragEnterWidgetEvent {

        widget: UIWidget;
        dragObject: UIDragObject;
    }

    export interface DragExitWidgetEvent {

        widget: UIWidget;
        dragObject: UIDragObject;
    }

    export interface DragEndedEvent {

        target: UIWidget;
        dragObject: UIDragObject;
    }

    export interface TemporaryChangedEvent {

        serializable: Atomic.Serializable;

    }

    export interface ComponentAddedEvent {

        scene: Atomic.Scene;
        node: Atomic.Node;
        component: Atomic.Component;

    }

    export interface ComponentRemovedEvent {

        scene: Atomic.Scene;
        node: Atomic.Node;
        component: Atomic.Component;

    }

    export interface IPCJSErrorEvent {

        errorName: string;
        errorMessage: string;
        errorFileName: string;
        errorLineNumber: number;
        errorStack: string;

    }

    export interface AttributeInfo {

        type: VariantType;
        name: string;
        mode: number; // AM_*
        defaultValue: string;
        enumNames: string[];
        resourceTypeName: string;

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

    export function getParentPath(path: string): string;
    export function addTrailingSlash(path: string): string;
    export function getExtension(path: string): string;

    export function splitPath(path: string): PathInfo;

}

declare module ToolCore {

    export interface ResourceAddedEvent {

        guid: string;

    }

    export interface ResourceRemovedEvent {

        guid: string;

    }

    export interface LicenseDeactivationErrorEvent {

        message: string;

    }

    export interface AssetImportErrorEvent {

        path:string;
        guid:string;
        error: string;

    }

    export var toolEnvironment: ToolEnvironment;
    export var toolSystem: ToolSystem;
    export var assetDatabase: AssetDatabase;
    export var licenseSystem: LicenseSystem;

    export function getToolEnvironment(): ToolEnvironment;
    export function getToolSystem(): ToolSystem;
    export function getAssetDatabase(): AssetDatabase;
    export function getLicenseSystem(): LicenseSystem;
}
