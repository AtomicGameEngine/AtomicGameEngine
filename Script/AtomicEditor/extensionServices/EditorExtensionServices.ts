//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import * as EditorEvents from "../editor/EditorEvents";
import TypescriptLanguageService from "./resourceServices/TypescriptLanguageService";
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
    save(ev: EditorEvents.SaveResourceEvent);
    canSave(ev: EditorEvents.SaveResourceEvent);
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
 * Registry for service extensions that are concerned about Resources
 */
class ResourceServiceRegistry extends ServiceRegistry<ResourceService> {
    saveResource(ev: EditorEvents.SaveResourceEvent) {
        // run through and find any services that can handle this.
        this.registeredServices.forEach((service) => {
            // Verify that the service contains the appropriate methods and that it can save
            if (service.canSave && service.save && service.canSave(ev)) {
                try {
                    service.save(ev);
                } catch (e) {
                    EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}\n ${e}`);
                }
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
    }

    resourceServices: ResourceServiceRegistry;

    loadService(service: EditorService) {
        service.initialize(this);
    }

    initializeServices() {
        // first we will load the built in services
        this.loadService(new TypescriptLanguageService);
        //TODO: Scan the user folders for services to load
    }
}
