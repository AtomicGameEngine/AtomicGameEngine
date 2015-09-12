//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

export const Quit = "EditorEventQuit";

export const ModalError = "ModalError";
export interface ModalErrorEvent {

  title: string;
  message: string;

}

export const PlayerStarted = "EditorPlayerStarted";
export const PlayerLog = "EditorPlayerLog";
export interface PlayerLogEvent {

  message: string;
  level: number;

}


export const ActiveSceneChange = "EditorActiveSceneChange";
export const ActiveNodeChange = "EditorActiveNodeChange";
export const SceneClosed = "EditorSceneClosed";
export interface SceneClosedEvent {

  scene: Atomic.Scene;

}

export const ContentFolderChanged = "ContentFolderChanged";
export interface ContentFolderChangedEvent {

  path: string;

}

export const CloseProject = "EditorCloseProject";

export const LoadProject = "EditorLoadProject";
export interface LoadProjectEvent {

  // The full path to the .atomic file
  path: string;

}

export const SaveAllResources = "EditorSaveAllResources";

export const SaveResource = "EditorSaveResource";
export interface SaveResourceEvent {

  // The full path to the resource to save
  // empty or undefined for current
  path: string;

}

export const CloseResource = "EditorCloseResource";
export interface CloseResourceEvent {

  editor:Editor.ResourceEditor;
  navigateToAvailableResource:boolean;

}

export const EditResource = "EditorEditResource";
export interface EditResourceEvent {

  // The full path to the resource to edit
  path: string;

}
