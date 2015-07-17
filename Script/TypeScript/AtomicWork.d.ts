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

    /*
        export enum UIWidgetEventType {

        EVENT_TYPE_CLICK,
        EVENT_TYPE_LONG_CLICK,
        EVENT_TYPE_POINTER_DOWN,
        EVENT_TYPE_POINTER_UP,
        EVENT_TYPE_POINTER_MOVE,
        EVENT_TYPE_RIGHT_POINTER_DOWN,
        EVENT_TYPE_RIGHT_POINTER_UP,
        EVENT_TYPE_WHEEL,
        EVENT_TYPE_CHANGED,
        EVENT_TYPE_KEY_DOWN,
        EVENT_TYPE_KEY_UP,
        EVENT_TYPE_SHORTCUT,
        EVENT_TYPE_CONTEXT_MENU,
        EVENT_TYPE_FILE_DROP,
        EVENT_TYPE_TAB_CHANGED,
        EVENT_TYPE_CUSTOM
        }
    */
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
