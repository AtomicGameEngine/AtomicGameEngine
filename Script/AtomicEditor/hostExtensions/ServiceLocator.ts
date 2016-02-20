//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//
import * as HostExtensionServices from "./HostExtensionServices";
import * as EditorUI from "../ui/EditorUI";
import TypescriptLanguageExtension from "./languageExtensions/TypscriptLanguageExtension";

/**
 * Generic service locator of editor services that may be injected by either a plugin
 * or by the editor itself.
 */
export class ServiceLocatorType implements Editor.HostExtensions.HostServiceLocator {

    constructor() {
        this.resourceServices = new HostExtensionServices.ResourceServiceRegistry();
        this.projectServices = new HostExtensionServices.ProjectServiceRegistry();
    }

    private eventDispatcher: Atomic.UIWidget = null;

    resourceServices: HostExtensionServices.ResourceServiceRegistry;
    projectServices: HostExtensionServices.ProjectServiceRegistry;

    loadService(service: Editor.HostExtensions.HostEditorService) {
        try {
            service.initialize(this);
        } catch (e) {
            EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}\n \n ${e.stack}`);
        }
    }

    /**
     * This is where the top level window will allow the service locator to listen for events and act on them.
     * @param  {Atomic.UIWidget} frame
     */
    subscribeToEvents(frame: Atomic.UIWidget) {
        this.eventDispatcher = frame;
        this.resourceServices.subscribeToEvents(this);
        this.projectServices.subscribeToEvents(this);
    }

    /**
     * Send a custom event.  This can be used by services to publish custom events
     * @param  {string} eventType
     * @param  {any} data
     */
    sendEvent(eventType: string, data: any) {
        if (this.eventDispatcher) {
            this.eventDispatcher.sendEvent(eventType, data);
        }
    }

    /**
     * Subscribe to an event and provide a callback.  This can be used by services to subscribe to custom events
     * @param  {string} eventType
     * @param  {any} callback
     */
    subscribeToEvent(eventType: string, callback: (data: any) => void) {
        if (this.eventDispatcher) {
            this.eventDispatcher.subscribeToEvent(eventType, callback);
        }
    }
}
// Singleton service locator that can be referenced
const serviceLocator = new ServiceLocatorType();
export default serviceLocator;

// Load up all the internal services
serviceLocator.loadService(new TypescriptLanguageExtension());
