/// <reference path="Atomic.d.ts" />
/// <reference path="ToolCore.d.ts" />
/// <reference path="Editor.d.ts" />


declare module Atomic {

    export function print(...args: any[]);

    export var platform: string;


    export interface PathInfo {

        pathName: string;
        fileName: string;
        ext: string;

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

    export interface AttributeInfo {

        type: VariantType;
        name: string;
        mode: number; // AM_*
        defaultValue: string;
        enumNames: string[];

    }

    export interface ShaderParameter {

        name: string;
        value: any;
        valueString: string;
        typeName: string;
        type: VariantType;

    }

    // subsystems

    export var engine:Engine;
    export var graphics:Graphics;
    export var renderer:Renderer;
    export var cache:ResourceCache;
    export var input:Input;
    export var fileSystem:FileSystem;
    export var network:Network;

    // end subsystems


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

declare module Editor {
    export function getPreferences(): AEPreferences;
}

declare module ToolCore {

    export interface ResourceAddedEvent {

        guid: string;

    }

    export interface ResourceRemovedEvent {

        guid: string;

    }

    export function getToolEnvironment(): ToolEnvironment;
    export function getToolSystem(): ToolSystem;
    export function getAssetDatabase(): AssetDatabase;
    export function getLicenseSystem(): LicenseSystem;
}
