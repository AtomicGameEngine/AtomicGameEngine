//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import * as EditorEvents from "../editor/EditorEvents";
import * as EditorUI from "../ui/EditorUI";

/**
 * Base interface for any editor services.
 */
export interface EditorService {
    /**
     * Called by the service locator at load time
     */
    initialize(serviceLocator: ServiceLocatorType);

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

export interface ResourceService extends EditorService {
    save?(ev: EditorEvents.SaveResourceEvent);
    canSave?(ev: EditorEvents.SaveResourceEvent);
    canDelete?(ev: EditorEvents.DeleteResourceEvent);
    delete?(ev: EditorEvents.DeleteResourceEvent);
    canRename?(ev: EditorEvents.RenameResourceEvent);
    rename?(ev: EditorEvents.RenameResourceEvent);
}

export interface ProjectService extends EditorService {
    projectUnloaded?();
    projectLoaded?(ev: EditorEvents.LoadProjectEvent);
    playerStarted?();
}

interface ServiceEventSubscriber {
    /**
     * Allow this service registry to subscribe to events that it is interested in
     * @param  {Atomic.UIWidget} topLevelWindow The top level window that will be receiving these events
     */
    subscribeToEvents(topLevelWindow: Atomic.UIWidget);
}

/**
 * Generic registry for storing Editor Extension Services
 */
class ServiceRegistry<T extends EditorService> {
    registeredServices: T[] = [];

    /**
     * Adds a service to the registered services list for this type of service
     * @param  {T}      service the service to register
     */
    register(service: T) {
        this.registeredServices.push(service);
    }

}

/**
 * Registry for service extensions that are concerned about project events
 */
class ProjectServiceRegistry extends ServiceRegistry<ProjectService> implements ServiceEventSubscriber {
    constructor() {
        super();
    }

    /**
     * Allow this service registry to subscribe to events that it is interested in
     * @param  {Atomic.UIWidget} topLevelWindow The top level window that will be receiving these events
     */
    subscribeToEvents(topLevelWindow: Atomic.UIWidget) {
        topLevelWindow.subscribeToEvent(EditorEvents.LoadProjectNotification, (ev) => this.projectLoaded(ev));
        topLevelWindow.subscribeToEvent(EditorEvents.CloseProject, (ev) => this.projectUnloaded(ev));
        topLevelWindow.subscribeToEvent(EditorEvents.PlayerStartRequest, () => this.playerStarted());
    }

    /**
     * Called when the project is unloaded
     * @param  {[type]} data Event info from the project unloaded event
     */
    projectUnloaded(data) {
        this.registeredServices.forEach((service) => {
            // Notify services that the project has been unloaded
            try {
                if (service.projectUnloaded) {
                    service.projectUnloaded();
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}\n ${e}\n ${e.stack}`);
            }
        });
    }

    /**
     * Called when the project is loaded
     * @param  {[type]} data Event info from the project unloaded event
     */
    projectLoaded(ev: EditorEvents.LoadProjectEvent) {
        this.registeredServices.forEach((service) => {
            try {
                // Notify services that the project has just been loaded
                if (service.projectLoaded) {
                    service.projectLoaded(ev);
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}\n ${e}\n ${e.stack}`);
            }
        });
    }

    playerStarted() {
        this.registeredServices.forEach((service) => {
            try {
                // Notify services that the project has just been loaded
                if (service.playerStarted) {
                    service.playerStarted();
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}\n ${e}\n ${e.stack}`);
            }
        });
    }
}

/**
 * Registry for service extensions that are concerned about Resources
 */
class ResourceServiceRegistry extends ServiceRegistry<ResourceService> {
    constructor() {
        super();
    }

    /**
     * Allow this service registry to subscribe to events that it is interested in
     * @param  {Atomic.UIWidget} topLevelWindow The top level window that will be receiving these events
     */
    subscribeToEvents(topLevelWindow: Atomic.UIWidget) {
        topLevelWindow.subscribeToEvent(EditorEvents.SaveResourceNotification, (ev) => this.saveResource(ev));
        topLevelWindow.subscribeToEvent(EditorEvents.DeleteResourceNotification, (ev) => this.deleteResource(ev));
        topLevelWindow.subscribeToEvent(EditorEvents.RenameResourceNotification, (ev) => this.renameResource(ev));
    }

    /**
     * Called after a resource has been saved
     * @param  {EditorEvents.SaveResourceEvent} ev
     */
    saveResource(ev: EditorEvents.SaveResourceEvent) {
        // run through and find any services that can handle this.
        this.registeredServices.forEach((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can save
                if (service.canSave && service.save && service.canSave(ev)) {
                    service.save(ev);
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}\n ${e}\n ${e.stack}`);
            }
        });
    }

    /**
     * Called when a resource has been deleted
     * @return {[type]} [description]
     */
    deleteResource(ev: EditorEvents.DeleteResourceEvent) {
        this.registeredServices.forEach((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can delete
                if (service.canDelete && service.delete && service.canDelete(ev)) {
                    service.delete(ev);
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}\n ${e}\n ${e.stack}`);
            }
        });
    }

    /**
     * Called when a resource has been renamed
     * @param  {EditorEvents.RenameResourceEvent} ev
     */
    renameResource(ev: EditorEvents.RenameResourceEvent) {
        this.registeredServices.forEach((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can handle the rename
                if (service.canRename && service.rename && service.canRename(ev)) {
                    service.rename(ev);
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}\n ${e}\n ${e.stack}`);
            }
        });
    }

}

/**
 * Generic service locator of editor services that may be injected by either a plugin
 * or by the editor itself.
 */
export class ServiceLocatorType {

    constructor() {
        this.resourceServices = new ResourceServiceRegistry();
        this.projectServices = new ProjectServiceRegistry();
    }

    private eventDispatcher: Atomic.UIWidget = null;

    resourceServices: ResourceServiceRegistry;
    projectServices: ProjectServiceRegistry;

    loadService(service: EditorService) {
        service.initialize(this);
    }

    /**
     * This is where the top level window will allow the service locator to listen for events and act on them.
     * @param  {Atomic.UIWidget} frame
     */
    subscribeToEvents(frame: Atomic.UIWidget) {
        this.eventDispatcher = frame;
        this.resourceServices.subscribeToEvents(this.eventDispatcher);
        this.projectServices.subscribeToEvents(this.eventDispatcher);
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
