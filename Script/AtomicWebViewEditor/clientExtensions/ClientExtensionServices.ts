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

import ClientExtensionEventNames from "./ClientExtensionEventNames";

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
class ServiceRegistry<T extends Editor.Extensions.ServiceEventListener> implements Editor.Extensions.ServicesProvider<T> {
    registeredServices: T[] = [];

    /**
     * Adds a service to the registered services list for this type of service
     * @param  {T}      service the service to register
     */
    register(service: T) {
        this.registeredServices.push(service);
    }

    unregister(service: T) {
        var index = this.registeredServices.indexOf(service, 0);
        if (index > -1) {
            this.registeredServices.splice(index, 1);
        }
    }
}

export class ExtensionServiceRegistry extends ServiceRegistry<Editor.ClientExtensions.WebViewService> {

    /**
     * Allow this service registry to subscribe to events that it is interested in
     * @param  {EventDispatcher} eventDispatcher The global event dispatcher
     */
    subscribeToEvents(eventDispatcher: Editor.Extensions.EventDispatcher) {
        eventDispatcher.subscribeToEvent(ClientExtensionEventNames.CodeLoadedEvent, (ev) => this.codeLoaded(ev));
        eventDispatcher.subscribeToEvent(ClientExtensionEventNames.ConfigureEditorEvent, (ev) => this.configureEditor(ev));
        eventDispatcher.subscribeToEvent(ClientExtensionEventNames.ResourceRenamedEvent, (ev) => this.renameResource(ev));
        eventDispatcher.subscribeToEvent(ClientExtensionEventNames.ProjectUnloadedEvent, (ev) => this.projectUnloaded());
        eventDispatcher.subscribeToEvent(ClientExtensionEventNames.ResourceDeletedEvent, (ev) => this.deleteResource(ev));
        eventDispatcher.subscribeToEvent(ClientExtensionEventNames.CodeSavedEvent, (ev) => this.saveCode(ev));
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
     * Called after code has been saved
     * @param  {Editor.EditorEvents.SaveResourceEvent} ev
     */
    saveCode(ev: Editor.EditorEvents.CodeSavedEvent) {
        // run through and find any services that can handle this.
        this.registeredServices.forEach((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can save
                if (service.save) {
                    service.save(ev);
                }
            } catch (e) {
                alert(`Error detected in extension ${service.name}\n \n ${e.stack}`);
            }
        });
    }

    /**
     * Called when a resource has been deleted
     */
    deleteResource(ev: Editor.EditorEvents.DeleteResourceEvent) {
        this.registeredServices.forEach((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can delete
                if (service.delete) {
                    service.delete(ev);
                }
            } catch (e) {
                alert(`Error detected in extension ${service.name}\n \n ${e.stack}`);
            }
        });
    }

    /**
     * Called when a resource has been renamed
     * @param  {Editor.EditorEvents.RenameResourceEvent} ev
     */
    renameResource(ev: Editor.EditorEvents.RenameResourceEvent) {
        this.registeredServices.forEach((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can handle the rename
                if (service.rename) {
                    service.rename(ev);
                }
            } catch (e) {
                alert(`Error detected in extension ${service.name}\n \n ${e.stack}`);
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


    /**
     * Called when the project is unloaded
     */
    projectUnloaded() {
        this.registeredServices.forEach((service) => {
            // Notify services that the project has been unloaded
            try {
                if (service.projectUnloaded) {
                    service.projectUnloaded();
                }
            } catch (e) {
                alert(`Extension Error:\n Error detected in extension ${service.name}\n \n ${e.stack}`);
            }
        });
    }
}
