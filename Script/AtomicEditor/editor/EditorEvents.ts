//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// TODO: migrate these interfaces out to the d.ts and migrate the static strings to some common location
export const ModalError = "ModalError";
export function ModalErrorEvent(callback: Atomic.EventCallback<ModalErrorEvent>) {
    return Atomic.ScriptEvent(ModalError, callback);
}
export interface ModalErrorEvent extends Atomic.EventMetaData {

  title: string;
  message: string;

}
export const PlayerStartRequest = "EditorPlayRequest";
export function PlayerStartRequestEvent(callback: Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(PlayerStartRequest, callback);
}
export const PlayerStarted = "EditorPlayerStarted";
export function PlayerStartedEvent(callback: Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(PlayerStarted, callback);
}

export const PlayerStopped = "EditorPlayerStopped";
export function PlayerStoppedEvent(callback: Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(PlayerStopped, callback);
}

export const PlayerPaused = "EditorPlayerPaused";
export function PlayerPausedEvent(callback: Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(PlayerPaused, callback);
}
export const PlayerResumed = "EditorPlayerResumed";
export function PlayerResumedEvent(callback: Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(PlayerPaused, callback);
}
export const PlayerLog = "EditorPlayerLog";
export function PlayerLogEvent(callback: Atomic.EventCallback<PlayerLogEvent>) {
    return Atomic.ScriptEvent(PlayerLog, callback);
}
export interface PlayerLogEvent extends Atomic.EventMetaData {

  message: string;
  level: number;

}


export const ActiveSceneEditorChange = "EditorActiveSceneEditorChange";
export function ActiveSceneEditorChangeEvent(callback: Atomic.EventCallback<ActiveSceneEditorChangeEvent>) {
    return Atomic.ScriptEvent(ActiveSceneEditorChange, callback);
}
export interface ActiveSceneEditorChangeEvent extends Atomic.EventMetaData {

  sceneEditor: Editor.SceneEditor3D;

}


export const SceneClosed = "EditorSceneClosed";
export function SceneClosedEvent(callback: Atomic.EventCallback<SceneClosedEvent>) {
    return Atomic.ScriptEvent(SceneClosed, callback);
}
export interface SceneClosedEvent extends Atomic.EventMetaData {

  scene: Atomic.Scene;

}

export const ContentFolderChanged = "ContentFolderChanged";
export function ContentFolderChangedEvent(callback: Atomic.EventCallback<ContentFolderChangedEvent>) {
    return Atomic.ScriptEvent(ContentFolderChanged, callback);
}
export interface ContentFolderChangedEvent extends Atomic.EventMetaData {

  path: string;

}

export const CloseProject = "EditorCloseProject";
export function CloseProjectEvent(callback: Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(CloseProject, callback);
}
export const ProjectClosed = "EditorProjectClosed";
export function ProjectClosedEvent(callback: Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(ProjectClosed, callback);
}
export const ProjectUnloadedNotification = "ProjectUnloadedNotification";
export function ProjectUnloadedNotificationEvent(callback: Atomic.EventCallback<null>){
    return Atomic.ScriptEvent(ProjectUnloadedNotification, callback);
}

export const RequestProjectLoad = "RequestProjectLoad";
export function RequestProjectLoadEvent(callback: Atomic.EventCallback<RequestProjectLoadEvent>) {
    return Atomic.ScriptEvent(RequestProjectLoad, callback);
}
export interface RequestProjectLoadEvent extends Atomic.EventMetaData {

  // The full path to the .atomic file
  path: string;

}

export const ProjectLoaded = "ProjectLoaded";
export function ProjectLoadedEvent(callback: Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(ProjectLoaded, callback);
}

export const LoadProject = "EditorLoadProject";
export function LoadProjectEvent(callback: Atomic.EventCallback<LoadProjectEvent>) {
    return Atomic.ScriptEvent(LoadProject, callback);
}
export const LoadProjectNotification = "EditorLoadProjectNotification";
export function LoadProjectNotificationEvent(callback: Atomic.EventCallback<LoadProjectEvent>) {
    return Atomic.ScriptEvent(LoadProjectNotification, callback);
}
export interface LoadProjectEvent extends Atomic.EventMetaData {

  // The full path to the .atomic file
  path: string;

}

export const SaveAllResources = "EditorSaveAllResources";
export function SaveAllResourcesEvent(callback: Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(SaveAllResources, callback);
}

export const SaveResource = "EditorSaveResource";
export function SaveResourceEvent(callback: Atomic.EventCallback<SaveResourceEvent>) {
    return Atomic.ScriptEvent(SaveResource, callback);
}
/**
 * Called once the resource has been saved
 * @type {String}
 */
export const SaveResourceNotification = "EditorSaveResourceNotification";
export function SaveResourceNotificationEvent(callback: Atomic.EventCallback<SaveResourceEvent>) {
    return Atomic.ScriptEvent(SaveResourceNotification, callback);
}
export interface SaveResourceEvent extends Atomic.EventMetaData {

  // The full path to the resource to save / empty or undefined for current
  path: string;

}

export const EditorResourceCloseCanceled = "EditorResourceCloseCanceled";
export function EditorResourceCloseCanceledEvent(callback: Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(EditorResourceCloseCanceled, callback);
}

export const EditorResourceClose = "EditorResourceClose";
export function EditorResourceCloseEvent(callback: Atomic.EventCallback<EditorCloseResourceEvent>) {
    return Atomic.ScriptEvent(EditorResourceClose, callback);
}
export interface EditorCloseResourceEvent extends Atomic.EventMetaData {

  editor:Editor.ResourceEditor;
  navigateToAvailableResource:boolean;

}

export const EditResource = "EditorEditResource";
export function EditResourceEvent(callback: Atomic.EventCallback<EditResourceEvent>) {
    return Atomic.ScriptEvent(EditResource, callback);
}
export interface EditResourceEvent extends Atomic.EventMetaData {

  // The full path to the resource to edit
  path: string;

}

export const DeleteResource = "EditorDeleteResource";
/**
 * Called once the resource has been deleted
 * @type {String}
 */
export const DeleteResourceNotification = "DeleteResourceNotification";
export function DeleteResourceNotificationEvent(callback: Atomic.EventCallback<DeleteResourceEvent>) {
    return Atomic.ScriptEvent(DeleteResourceNotification, callback);
}
export interface DeleteResourceEvent extends Atomic.EventMetaData {

  // The full path to the resource to edit
  path: string;

}

export const RenameResource = "EditorRenameResource";
/**
 * Called once the resource has been renamed
 * @type {String}
 */
export const RenameResourceNotification = "RenameResourceNotification";
export function RenameResourceNotificationEvent(callback: Atomic.EventCallback<RenameResourceEvent>) {
    return Atomic.ScriptEvent(RenameResourceNotification, callback);
}
export interface RenameResourceEvent extends Atomic.EventMetaData {

  // The full path to the resource to edit
  path: string;
  newPath: string;
  newName: string;

  // the asset to delete
  asset: ToolCore.Asset;
}

export const SceneEditStateChange = "SceneEditStateChange";
export interface SceneEditStateChangeEvent {

  serializable: Atomic.Serializable;

}

export const InspectorProjectReference = "InspectorProjectReference";
export function InspectorProjectReferenceEvent(callback : Atomic.EventCallback<InspectorProjectReferenceEvent>) {
    return Atomic.ScriptEvent(InspectorProjectReference, callback);
}
export interface InspectorProjectReferenceEvent extends Atomic.EventMetaData {

    // The full path to the resource to edit
    path: string;

}

export const UserPreferencesChangedNotification  = "UserPreferencesChangedNotification";
export function UserPreferencesChangedNotificationEvent(callback : Atomic.EventCallback<UserPreferencesChangedEvent>) {
    return Atomic.ScriptEvent(UserPreferencesChangedNotification, callback);
}
export interface UserPreferencesChangedEvent extends Atomic.EventMetaData {
    /**
     * JSON string of the project preferences
     */
    projectPreferences: string;

    /**
     * JSON string of the application preferences
     */
    applicationPreferences: string;
}

export const WebViewLoadEnd = "WebViewLoadEnd";
export function WebViewLoadEndEvent(callback : Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(WebViewLoadEnd, callback);
}
export const WebMessage = "WebMessage";
export function WebMessageEvent(callback: Atomic.EventCallback<WebMessageEvent>) {
    return Atomic.ScriptEvent(WebMessage, callback);
}
export interface WebMessageEvent extends Atomic.EventMetaData {
    /**
     * Could be either a JSON formatted object or a simple string
     * @type {string}
     */
    request: string;
}

// interface to pass modal error messages from core modules
export const EditorModal = "EditorModal";
export function EditorModalEvent(callback: Atomic.EventCallback<EditorModalEvent>) {
    return Atomic.ScriptEvent(EditorModal, callback);
}
export interface EditorModalEvent extends Atomic.EventMetaData {
    uiType: number;     // EDITOR_ERROR_MODAL, etc)
    title: string;      // for modal errors, title text
    message: string;    // for modal errors, error text
}

export const AttributeEditResourceChangedEventName = "AttributeEditResourceChanged";
export function AttributeEditResourceChangedEvent(callback: Atomic.EventCallback<AttributeEditResourceChangedEvent>) {
    return Atomic.ScriptEvent(AttributeEditResourceChangedEventName, callback);
}

export interface AttributeEditResourceChangedEvent extends Atomic.EventMetaData {

    //attrInfoEdit: AttributeInfoEdit;
    resource: Atomic.Resource;

}
