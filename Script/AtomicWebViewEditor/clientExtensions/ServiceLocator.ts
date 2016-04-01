//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//
import HostInteropType from "../interop";
import * as ClientExtensionServices from "./ClientExtensionServices";

// Initialize and configure the extensions
import tsExtension from "./languageExtensions/typescript/TypescriptLanguageExtension";
import jsExtension from "./languageExtensions/javascript/JavascriptLanguageExtension";

/**
 * Generic service locator of editor services that may be injected by either a plugin
 * or by the editor itself.
 */
export class ClientServiceLocatorType implements Editor.ClientExtensions.ClientServiceLocator {

    constructor() {
        this.services = new ClientExtensionServices.ExtensionServiceRegistry();
        this.services.subscribeToEvents(this);
    }

    private services: ClientExtensionServices.ExtensionServiceRegistry;
    private eventDispatcher: Editor.Extensions.EventDispatcher = new ClientExtensionServices.EventDispatcher();

    /**
     * Returns the Host Interop module
     * @return {Editor.ClientExtensions.HostInterop}
     */
    getHostInterop(): Editor.ClientExtensions.HostInterop {
        return HostInteropType.getInstance();
    }

    loadService(service: Editor.ClientExtensions.ClientEditorService) {
        try {
            this.services.register(service);
            service.initialize(this);
        } catch (e) {
            alert(`Extension Error:\n Error detected in extension ${service.name}\n \n ${e.stack}`);
        }
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
const serviceLocator = new ClientServiceLocatorType();
export default serviceLocator;

// Load up all the internal services
serviceLocator.loadService(new tsExtension());
serviceLocator.loadService(new jsExtension());
