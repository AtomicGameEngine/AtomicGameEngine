//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

// Entry point for web view extensions -- extensions that live inside the web view

interface EventSubscription {
    eventName: string;
    callback: (data: any) => any;
}

/**
 * Implements an event dispatcher for the client services
 */
export class EventDispatcher implements Editor.Extensions.EventDispatcher {
    private subscriptions: EventSubscription[] = [];

    sendEvent(eventType: string, data: any) {
        this.subscriptions.forEach(sub => {
            if (sub.eventName == eventType) {
                sub.callback(data);
            }
        });
    }

    subscribeToEvent(eventType, callback) {
        this.subscriptions.push({
            eventName: eventType,
            callback: callback
        });
    }
}

/**
 * Generic registry for storing Editor Extension Services
 */
class ServiceRegistry<T extends Editor.Extensions.EditorService> implements Editor.Extensions.ServiceRegistry<T> {
    registeredServices: T[] = [];

    /**
     * Adds a service to the registered services list for this type of service
     * @param  {T}      service the service to register
     */
    register(service: T) {
        this.registeredServices.push(service);
    }
}

export class ExtensionServiceRegistry extends ServiceRegistry<Editor.ClientExtensions.WebViewService> {

    /**
     * Allow this service registry to subscribe to events that it is interested in
     * @param  {EventDispatcher} eventDispatcher The global event dispatcher
     */
    subscribeToEvents(eventDispatcher: Editor.Extensions.EventDispatcher) {
        eventDispatcher.subscribeToEvent("CodeLoadedNotification", (ev) => this.codeLoaded(ev));
        eventDispatcher.subscribeToEvent("ConfigureEditor", (ev) => this.configureEditor(ev));
    }

    /**
     * Called when code is loaded
     * @param  {Editor.EditorEvents.CodeLoadedEvent} ev Event info about the file that is being loaded
     */
    codeLoaded(ev: Editor.EditorEvents.CodeLoadedEvent) {
        this.registeredServices.forEach((service) => {
            try {
                // Notify services that the project has just been loaded
                if (service.codeLoaded) {
                    service.codeLoaded(ev);
                }
            } catch (e) {
                alert(`Extension Error:\n Error detected in extension ${service.name}\n \n ${e.stack}`);
            }
        });
    }

    /**
     * Called when the editor is requesting to be configured for a particular file
     * @param  {Editor.EditorEvents.EditorFileEvent} ev
     */
    configureEditor(ev: Editor.EditorEvents.EditorFileEvent) {
        this.registeredServices.forEach((service) => {
            try {
                // Notify services that the project has just been loaded
                if (service.configureEditor) {
                    service.configureEditor(ev);
                }
            } catch (e) {
                alert(`Extension Error:\n Error detected in extension ${service.name}\n \n ${e.stack}`);
            }
        });
    }
}
