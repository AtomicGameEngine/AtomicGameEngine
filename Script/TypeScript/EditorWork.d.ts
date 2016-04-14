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
    export interface EditorServiceExtension {
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

    /**
     * Base Service Event Listener.  Attach descendents of these to an EditorServiceExtension
     * to hook service events
     */
    export interface ServiceEventListener extends EditorServiceExtension { }

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
        loadService(service: EditorServiceExtension): void;
    }

    /**
     * Service registry interface for registering services
     */
    export interface ServicesProvider<T extends ServiceEventListener> {
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
        resourceServices: ResourceServicesProvider;
        projectServices: ProjectServicesProvider;
        uiServices: UIServicesProvider;
    }

    export interface HostEditorService extends Editor.Extensions.EditorServiceExtension {
        /**
         * Called by the service locator at load time
         */
        initialize(serviceLocator: HostServiceLocator);
    }

    export interface ResourceServicesEventListener extends Editor.Extensions.ServiceEventListener {
        save?(ev: EditorEvents.SaveResourceEvent);
        delete?(ev: EditorEvents.DeleteResourceEvent);
        rename?(ev: EditorEvents.RenameResourceEvent);
    }
    export interface ResourceServicesProvider extends Editor.Extensions.ServicesProvider<ResourceServicesEventListener> { }

    export interface ProjectServicesEventListener extends Editor.Extensions.ServiceEventListener {
        projectUnloaded?();
        projectLoaded?(ev: EditorEvents.LoadProjectEvent);
        playerStarted?();
    }
    export interface ProjectServicesProvider extends Editor.Extensions.ServicesProvider<ProjectServicesEventListener> { }

    export interface UIServicesEventListener extends Editor.Extensions.ServiceEventListener {
        menuItemClicked?(refId: string): boolean;
    }
    export interface UIServicesProvider extends Editor.Extensions.ServicesProvider<UIServicesEventListener> {
        createPluginMenuItemSource(id: string, items: any): Atomic.UIMenuItemSource;
        removePluginMenuItemSource(id: string);
        showModalWindow(windowText: string, uifilename: string, handleWidgetEventCB: (ev: Atomic.UIWidgetEvent) => void): Editor.Modal.ExtensionWindow;
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

    export interface ClientEditorService extends Editor.Extensions.EditorServiceExtension {
        /**
         * Called by the service locator at load time
         */
        initialize(serviceLocator: ClientServiceLocator);
    }

    export interface WebViewService extends Editor.Extensions.EditorServiceExtension {
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
