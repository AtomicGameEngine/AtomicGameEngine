/// <reference path="/Users/josh/Dev/thunderbeast/AtomicGameEngine/Bin/Atomic.d.ts" />
/// <reference path="/Users/josh/Dev/thunderbeast/AtomicGameEngine/Bin/ToolCore.d.ts" />
/// <reference path="/Users/josh/Dev/thunderbeast/AtomicGameEngine/Bin/Editor.d.ts" />

declare module Atomic
{
	export function getInput() : Input;
	export function getGraphics() : Graphics;
	export function getFileSystem(); FileSystem;
}

declare module ToolCore
{
	export function getToolEnvironment() : ToolEnvironment;
	export function getToolSystem() : ToolSystem;
}
