//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

/// <reference path="Atomic.d.ts" />
/// <reference path="Editor.d.ts" />

declare module Editor.EditorEvents {

    export interface ModalErrorEvent {

        title: string;
        message: string;

    }

    export interface PlayerLogEvent {

        message: string;
        level: number;

    }

    export interface ActiveSceneEditorChangeEvent {

        sceneEditor: Editor.SceneEditor3D;

    }

    export interface SceneClosedEvent {

        scene: Atomic.Scene;

    }

    export interface ContentFolderChangedEvent {

        path: string;

    }

    export interface LoadProjectEvent {

        // The full path to the .atomic file
        path: string;

    }

    /**
     * Called once the resource has been saved
     * @type {String}
     */
    export interface SaveResourceEvent {

        // The full path to the resource to save / empty or undefined for current
        path: string;

    }

    export interface LoadResourceEvent {

        // The full path to the resource to load
        path: string;

    }

    export interface EditorFileEvent {
        filename: string;
        fileExt: string;
        editor: any;
    }

    export interface CodeLoadedEvent extends EditorFileEvent {
        code: string;
    }

    export interface CodeSavedEvent extends EditorFileEvent {
        code: string;
    }

    export interface EditorCloseResourceEvent {

        editor: Editor.ResourceEditor;
        navigateToAvailableResource: boolean;

    }

    export interface EditResourceEvent {

        // The full path to the resource to edit
        path: string;

    }

    /**
     * Called once the resource has been deleted
     * @type {String}
     */
    export interface DeleteResourceEvent {

        // The full path to the resource to edit
        path: string;

    }

    /**
     * Called once the resource has been renamed
     * @type {String}
     */
    export interface RenameResourceEvent {

        /**
         * Original path of the resource
         * @type {string}
         */
        path: string;

        /**
         * New path of the resource
         * @type {string}
         */
        newPath: string;

        /**
         * New base name of the resource (no path or extension)
         * @type {string}
         */
        newName?: string;

        // the asset being changed
        asset?: ToolCore.Asset;
    }

    export interface SceneEditStateChangeEvent {

        serializable: Atomic.Serializable;

    }
}

declare module Editor.Extensions {

    /**
     * Base interface for any editor services.
     */
    export interface EditorService {
        /**
         * Unique name of this service
         * @type {string}
         */
        name: string;

        /**
         * Description of this service
         * @type {string}
         */
        description: string;

    }

    interface EventDispatcher {
        /**
         * Send a custom event.  This can be used by services to publish custom events
         * @param  {string} eventType
         * @param  {any} data
         */
        sendEvent(eventType: string, data: any);

        /**
         * Subscribe to an event and provide a callback.  This can be used by services to subscribe to custom events
         * @param  {string} eventType
         * @param  {any} callback
         */
        subscribeToEvent(eventType, callback);
    }

    /**
     * Generic service locator of editor services that may be injected by either a plugin
     * or by the editor itself.
     */
    export interface ServiceLoader extends EventDispatcher {
        /**
         * Loads a service into a service registry
         * @param  {EditorService} service
         */
        loadService(service: EditorService): void;
    }

    /**
     * Service registry interface for registering services
     */
    export interface ServiceRegistry<T extends EditorService> {
        registeredServices: T[];

        /**
         * Adds a service to the registered services list for this type of service
         * @param  {T}      service the service to register
         */
        register(service: T);
        /**
         * Removes a service from the registered services list for this type of service
         * @param  {T}      service the service to unregister
         */
        unregister(service: T);
    }
}

declare module Editor.Modal {
    export interface ExtensionWindow extends Atomic.UIWindow {
        hide();
    }
}

declare module Editor.HostExtensions {

    /**
     * Generic service locator of editor services that may be injected by either a plugin
     * or by the editor itself.
     */
    export interface HostServiceLocator extends Editor.Extensions.ServiceLoader {
        resourceServices: ResourceServiceRegistry;
        projectServices: ProjectServiceRegistry;
        uiServices: UIServiceRegistry;
    }

    export interface HostEditorService extends Editor.Extensions.EditorService {
        /**
         * Called by the service locator at load time
         */
        initialize(serviceLocator: Editor.Extensions.ServiceLoader);
    }

    export interface ResourceService extends Editor.Extensions.EditorService {
        save?(ev: EditorEvents.SaveResourceEvent);
        delete?(ev: EditorEvents.DeleteResourceEvent);
        rename?(ev: EditorEvents.RenameResourceEvent);
    }
    export interface ResourceServiceRegistry extends Editor.Extensions.ServiceRegistry<ResourceService> { }

    export interface ProjectService extends Editor.Extensions.EditorService {
        projectUnloaded?();
        projectLoaded?(ev: EditorEvents.LoadProjectEvent);
        playerStarted?();
    }
    export interface ProjectServiceRegistry extends Editor.Extensions.ServiceRegistry<ProjectService> { }

    export interface UIService extends Editor.Extensions.EditorService {
        menuItemClicked?(refId: string): boolean;
    }
    export interface UIServiceRegistry extends Editor.Extensions.ServiceRegistry<UIService> {
        createPluginMenuItemSource(id: string, items: any): Atomic.UIMenuItemSource;
        removePluginMenuItemSource(id: string);
        showModalWindow(windowText: string, uifilename: string, handleWidgetEventCB: (ev: Atomic.UIWidgetEvent) => void): Editor.Modal.ExtensionWindow;
        menuItemClicked(refId: string): boolean;
    }
}

/**
 * Interfaces for client extensions
 */
declare module Editor.ClientExtensions {

    /**
     * Generic service locator of editor services that may be injected by either a plugin
     * or by the editor itself.
     */
    export interface ClientServiceLocator extends Editor.Extensions.ServiceLoader {
        getHostInterop(): HostInterop;
    }

    export interface ClientEditorService extends Editor.Extensions.EditorService {
        /**
         * Called by the service locator at load time
         */
        initialize(serviceLocator: ClientServiceLocator);
    }

    export interface WebViewService extends Editor.Extensions.EditorService {
        configureEditor?(ev: EditorEvents.EditorFileEvent);
        codeLoaded?(ev: EditorEvents.CodeLoadedEvent);
        save?(ev: EditorEvents.CodeSavedEvent);
        delete?(ev: EditorEvents.DeleteResourceEvent);
        rename?(ev: EditorEvents.RenameResourceEvent);
        projectUnloaded?();
    }

    export interface AtomicErrorMessage {
        error_code: number;
        error_message: string;
    }

    /**
     * Interface for functions that are available from the client web view to call on the host
     */
    export interface HostInterop {

        /**
         * Called from the host to notify the client what file to load
         * @param  {string} codeUrl
         */
        loadCode(codeUrl: string);

        /**
         * Save the contents of the editor
         * @return {Promise}
         */
        saveCode(): PromiseLike<{}>;

        /**
         * Save the contents of a file as filename
         * @param  {string} filename
         * @param  {string} fileContents
         * @return {Promise}
         */
        saveFile(filename: string, fileContents: string): PromiseLike<{}>;

        /**
         * Queries the host for a particular resource and returns it in a promise
         * @param  {string} codeUrl
         * @return {Promise}
         */
        getResource(codeUrl: string): PromiseLike<{}>;

        /**
         * Returns a file resource from the resources directory
         * @param  {string} filename name and path of file under the project directory or a fully qualified file name
         * @return {Promise}
         */
        getFileResource(filename: string): PromiseLike<{}>;

        /**
         * Notify the host that the contents of the editor has changed
         */
        notifyEditorChange();
    }
}
