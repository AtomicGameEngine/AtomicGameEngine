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

import * as EditorEvents from "../editor/EditorEvents";
import * as EditorUI from "../ui/EditorUI";
import MainFrame = require("../ui/frames/MainFrame");
import ModalOps = require("../ui/modal/ModalOps");
import ResourceOps = require("../resources/ResourceOps");
import Editor = require("../editor/Editor");

/**
 * Generic registry for storing Editor Extension Services
 */
export class ServicesProvider<T extends Editor.Extensions.ServiceEventListener> implements Editor.Extensions.ServicesProvider<T> {
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

export interface ServiceEventSubscriber {
    /**
     * Allow this service registry to subscribe to events that it is interested in
     * @param  {Atomic.UIWidget} topLevelWindow The top level window that will be receiving these events
     */
    subscribeToEvents(topLevelWindow: Atomic.UIWidget);
}

/**
 * Registry for service extensions that are concerned about project events
 */
export class ProjectServicesProvider extends ServicesProvider<Editor.HostExtensions.ProjectServicesEventListener> implements Editor.HostExtensions.ProjectServicesProvider {
    constructor() {
        super();
    }

    /**
     * Allow this service registry to subscribe to events that it is interested in
     * @param  {Atomic.UIWidget} topLevelWindow The top level window that will be receiving these events
     */
    subscribeToEvents(eventDispatcher: Editor.Extensions.EventDispatcher) {
        eventDispatcher.subscribeToEvent(EditorEvents.LoadProjectNotification, (ev) => this.projectLoaded(ev));
        eventDispatcher.subscribeToEvent(EditorEvents.CloseProject, (ev) => this.projectUnloaded(ev));
        eventDispatcher.subscribeToEvent(EditorEvents.PlayerStartRequest, () => this.playerStarted());
    }

    /**
     * Called when the project is unloaded
     * @param  {[type]} data Event info from the project unloaded event
     */
    projectUnloaded(data) {
        // Need to use a for loop for length down to 0 because extensions *could* delete themselves from the list on projectUnloaded
        for (let i = this.registeredServices.length - 1; i >= 0; i--) {
            let service = this.registeredServices[i];
            // Notify services that the project has been unloaded
            try {
                if (service.projectUnloaded) {
                    service.projectUnloaded();
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e} \n\n ${e.stack}`);
            }
        };
    }

    /**
     * Called when the project is loaded
     * @param  {[type]} data Event info from the project unloaded event
     */
    projectLoaded(ev: Editor.EditorEvents.LoadProjectEvent) {
        // Need to use a for loop and don't cache the length because the list of services *may* change while processing.  Extensions could be appended to the end
        for (let i = 0; i < this.registeredServices.length; i++) {
            let service = this.registeredServices[i];
            try {
                // Notify services that the project has just been loaded
                if (service.projectLoaded) {
                    service.projectLoaded(ev);
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e}\n\n ${e.stack}`);
            }
        };
    }

    playerStarted() {
        this.registeredServices.forEach((service) => {
            try {
                // Notify services that the project has just been loaded
                if (service.playerStarted) {
                    service.playerStarted();
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}\n \n ${e.stack}`);
            }
        });
    }

    /**
     * Return a preference value or the provided default from the user settings file
     * @param  {string} extensionName name of the extension the preference lives under
     * @param  {string} preferenceName name of the preference to retrieve
     * @param  {number | boolean | string} defaultValue value to return if pref doesn't exist
     * @return {number|boolean|string}
     */
    getUserPreference(settingsGroup: string, preferenceName: string, defaultValue?: number): number;
    getUserPreference(settingsGroup: string, preferenceName: string, defaultValue?: string): string;
    getUserPreference(settingsGroup: string, preferenceName: string, defaultValue?: boolean): boolean;
    getUserPreference(extensionName: string, preferenceName: string, defaultValue?: any): any {
        return EditorUI.getEditor().getUserPreference(extensionName, preferenceName, defaultValue);
    }


    /**
     * Sets a user preference value in the user settings file
     * @param  {string} extensionName name of the extension the preference lives under
     * @param  {string} preferenceName name of the preference to set
     * @param  {number | boolean | string} value value to set
     */
    setUserPreference(extensionName: string, preferenceName: string, value: number | boolean | string) {
        EditorUI.getEditor().setUserPreference(extensionName, preferenceName, value);
    }

    /**
     * Sets a group of user preference values in the user settings file located in the project.  Elements in the
     * group will merge in with existing group preferences.  Use this method if setting a bunch of settings
     * at once.
     * @param  {string} extensionName name of the group the preference lives under
     * @param  {string} groupPreferenceValues an object literal containing all of the preferences for the group.
     */
    setUserPreferenceGroup(extensionName: string, groupPreferenceValues: Object) {
        EditorUI.getEditor().setUserPreferenceGroup(extensionName, groupPreferenceValues);
    }
}

/**
 * Registry for service extensions that are concerned about Resources
 */
export class ResourceServicesProvider extends ServicesProvider<Editor.HostExtensions.ResourceServicesEventListener> implements Editor.HostExtensions.ResourceServicesProvider {
    constructor() {
        super();
    }

    /**
     * Allow this service registry to subscribe to events that it is interested in
     * @param  {Atomic.UIWidget} topLevelWindow The top level window that will be receiving these events
     */
    subscribeToEvents(eventDispatcher: Editor.Extensions.EventDispatcher) {
        eventDispatcher.subscribeToEvent(EditorEvents.SaveResourceNotification, (ev) => this.saveResource(ev));
        eventDispatcher.subscribeToEvent(EditorEvents.DeleteResourceNotification, (ev) => this.deleteResource(ev));
        eventDispatcher.subscribeToEvent(EditorEvents.RenameResourceNotification, (ev) => this.renameResource(ev));
        eventDispatcher.subscribeToEvent(EditorEvents.EditResource, (ev) => this.editResource(ev));

    }

    /**
     * Called after a resource has been saved
     * @param  {Editor.EditorEvents.SaveResourceEvent} ev
     */
    saveResource(ev: Editor.EditorEvents.SaveResourceEvent) {
        // run through and find any services that can handle this.
        this.registeredServices.forEach((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can save
                if (service.save) {
                    service.save(ev);
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e}\n\n ${e.stack}`);
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
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e}\n\n ${e.stack}`);
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
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e}\n\n ${e.stack}`);
            }
        });
    }

    /**
     * Called when a resource is about to be edited
     * @param  {Editor.EditorEvents.EditResourceEvent} ev
     */
    editResource(ev: Editor.EditorEvents.EditResourceEvent) {
        this.registeredServices.forEach((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can handle the edit
                if (service.edit) {
                    service.edit(ev);
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e}\n\n ${e.stack}`);
            }
        });
    }

    /**
     * Create New Material
     * @param  {string} resourcePath
     * @param  {string} materialName
     * @param  {boolean} reportError
     */
    createMaterial(resourcePath: string, materialName: string, reportError: boolean): boolean {
        return ResourceOps.CreateNewMaterial(resourcePath, materialName, reportError);
    }

}

/**
 * Registry for service extensions that are concerned about Scenes
 */
export class SceneServicesProvider extends ServicesProvider<Editor.HostExtensions.SceneServicesEventListener> implements Editor.HostExtensions.SceneServicesProvider {
    constructor() {
        super();
    }

    /**
     * Allow this service registry to subscribe to events that it is interested in
     * @param  {Atomic.UIWidget} topLevelWindow The top level window that will be receiving these events
     */
    subscribeToEvents(eventDispatcher: Editor.Extensions.EventDispatcher) {
        eventDispatcher.subscribeToEvent(EditorEvents.ActiveSceneEditorChange, (ev) => this.activeSceneEditorChange(ev));
        eventDispatcher.subscribeToEvent(EditorEvents.SceneClosed, (ev) => this.sceneClosed(ev));
    }

    /**
     * Called after an active scene editor change
     * @param  {Editor.EditorEvents.ActiveSceneEditorChangeEvent} ev
     */
    activeSceneEditorChange(ev: Editor.EditorEvents.ActiveSceneEditorChangeEvent) {
        // run through and find any services that can handle this.
        this.registeredServices.forEach((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can save
                if (service.activeSceneEditorChanged) {
                    service.activeSceneEditorChanged(ev);
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e}\n\n ${e.stack}`);
            }
        });
    }

    /**
     * Called after a scene is closed
     * @param  {Editor.EditorEvents.SceneClosedEvent} ev
     */
    sceneClosed(ev: Editor.EditorEvents.SceneClosedEvent) {
        // run through and find any services that can handle this.
        this.registeredServices.forEach((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can save
                if (service.editorSceneClosed) {
                    service.editorSceneClosed(ev);
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e}\n\n ${e.stack}`);
            }
        });
    }
}

/**
 * Registry for service extensions that are concerned about and need access to parts of the editor user interface
 * Note: we may want to move this out into it's own file since it has a bunch of editor dependencies
 */
export class UIServicesProvider extends ServicesProvider<Editor.HostExtensions.UIServicesEventListener> implements Editor.HostExtensions.UIServicesProvider {
    constructor() {
        super();
    }

    private mainFrame: MainFrame = null;
    private modalOps: ModalOps;

    init(mainFrame: MainFrame, modalOps: ModalOps) {
        // Only set these once
        if (this.mainFrame == null) {
            this.mainFrame = mainFrame;
        }
        if (this.modalOps == null) {
            this.modalOps = modalOps;
        }
    }

    /**
     * Adds a new menu to the plugin menu
     * @param  {string} id
     * @param  {any} items
     * @return {Atomic.UIMenuItemSource}
     */
    createPluginMenuItemSource(id: string, items: any): Atomic.UIMenuItemSource {
        return this.mainFrame.menu.createPluginMenuItemSource(id, items);
    }

    /**
     * Removes a previously added menu from the plugin menu
     * @param  {string} id
     */
    removePluginMenuItemSource(id: string) {
        this.mainFrame.menu.removePluginMenuItemSource(id);
    }


    /**
     * Returns the currently active resource editor or null
     * @return {Editor.ResourceEditor}
     */
    getCurrentResourceEditor(): Editor.ResourceEditor {
        return this.mainFrame.resourceframe.currentResourceEditor;
    }

    /**
     * Adds a new menu to the hierarchy context menu
     * @param  {string} id
     * @param  {any} items
     * @return {Atomic.UIMenuItemSource}
     */
    createHierarchyContextMenuItemSource(id: string, items: any): Atomic.UIMenuItemSource {
        return this.mainFrame.hierarchyFrame.menu.createPluginItemSource(id, items);
    }

    /**
     * Removes a previously added menu from the hierarchy context menu
     * @param  {string} id
     */
    removeHierarchyContextMenuItemSource(id: string) {
        this.mainFrame.hierarchyFrame.menu.removePluginItemSource(id);
    }

    /**
     * Adds a new menu to the project context menu
     * @param  {string} id
     * @param  {any} items
     * @return {Atomic.UIMenuItemSource}
     */
    createProjectContextMenuItemSource(id: string, items: any): Atomic.UIMenuItemSource {
        return this.mainFrame.projectframe.menu.createPluginItemSource(id, items);
    }

    /**
     * Removes a previously added menu from the project context menu
     * @param  {string} id
     */
    removeProjectContextMenuItemSource(id: string) {
        this.mainFrame.projectframe.menu.removePluginItemSource(id);
    }

    /**
     * Refreshes the hierarchy frame
     */
    refreshHierarchyFrame() {
        this.mainFrame.hierarchyFrame.populate();
    }

    /**
     * Disaplays a modal window
     * @param  {Editor.Modal.ModalWindow} window
     */
    showModalWindow(windowText: string, uifilename: string, handleWidgetEventCB: (ev: Atomic.UIWidgetEvent) => void): Editor.Modal.ExtensionWindow {
        return this.modalOps.showExtensionWindow(windowText, uifilename, handleWidgetEventCB);
    }

    /**
     * Disaplays a modal error window
     * @param  {string} windowText
     * @param  {string} message
     */
    showModalError(windowText: string, message: string) {
        return this.modalOps.showError(windowText, message);
    }

    /**
     * Displays a resource slection window
     * @param  {string} windowText
     * @param  {string} importerType
     * @param  {string} resourceType
     * @param  {function} callback
     * @param  {any} retObject
     * @param  {any} args
     */
    showResourceSelection(windowText: string, importerType: string, resourceType: string, callback: (retObject: any, args: any) => void, args: any = undefined) {

        this.modalOps.showResourceSelection(windowText, importerType, resourceType, callback);
    }

    /**
     * Will register a custom editor for a particular file type.
     * @param  {Editor.Extensions.ResourceEditorBuilder} editorBuilder
     */
    registerCustomEditor(editorBuilder: Editor.Extensions.ResourceEditorBuilder) {
        this.mainFrame.resourceframe.resourceEditorProvider.registerCustomEditor(editorBuilder);
    }

    /**
     * Will unregister a previously registered editor builder
     * @param  {Editor.Extensions.ResourceEditorBuilder} editorBuilder
     */
    unregisterCustomEditor(editorBuilder: Editor.Extensions.ResourceEditorBuilder) {
        this.mainFrame.resourceframe.resourceEditorProvider.unregisterCustomEditor(editorBuilder);
    }

    /**
     * Called when a menu item has been clicked
     * @param  {string} refId
     * @type {boolean} return true if handled
     */
    menuItemClicked(refid: string): boolean {
        // run through and find any services that can handle this.
        return this.registeredServices.some((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can handle it
                if (service.menuItemClicked) {
                    if (service.menuItemClicked(refid)) {
                        return true;
                    }
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e}\n\n ${e.stack}`);
            }
        });
    }

    /**
     * Called when a context menu item in the hierarchy pane has been clicked
     * @param  {Atomic.Node} node
     * @param  {string} refId
     * @type {boolean} return true if handled
     */
    hierarchyContextItemClicked(node: Atomic.Node, refid: string): boolean {
        if (!node)
            return false;

        // run through and find any services that can handle this.
        return this.registeredServices.some((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can handle it
                if (service.hierarchyContextItemClicked) {
                    if (service.hierarchyContextItemClicked(node, refid)) {
                        return true;
                    }
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e}\n\n ${e.stack}`);
            }
        });
    }


    /**
     * Called when a context menu item in the hierarchy pane has been clicked
     * @param  {ToolCore.Asset} asset
     * @param  {string} refId
     * @type {boolean} return true if handled
     */
    projectContextItemClicked(asset: ToolCore.Asset, refid: string): boolean {
        if (!asset)
            return false;

        // run through and find any services that can handle this.
        return this.registeredServices.some((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can handle it
                if (service.projectContextItemClicked) {
                    if (service.projectContextItemClicked(asset, refid)) {
                        return true;
                    }
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e}\n\n ${e.stack}`);
            }
        });
    }

    /**
     * Hooks into web messages coming in from web views
     * @param  {[String|Object]} data
     */
    handleWebMessage(data) {
        let messageType;
        let messageObject;

        try {
            messageObject = JSON.parse(data.request);
            messageType = messageObject.message;
        } catch (e) {
            // not JSON, we are just getting a notification message of some sort
            messageType = data.request;
        }

        // run through and find any services that can handle this.
        this.registeredServices.forEach((service) => {
            try {
                // Verify that the service contains the appropriate methods and that it can save
                if (service.handleWebMessage) {
                    service.handleWebMessage(messageType, messageObject);
                }
            } catch (e) {
                EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e}\n\n ${e.stack}`);
            }
        });
    }

    /**
     * Allow this service registry to subscribe to events that it is interested in
     * @param  {Atomic.UIWidget} topLevelWindow The top level window that will be receiving these events
     */
    subscribeToEvents(eventDispatcher: Editor.Extensions.EventDispatcher) {
        // Placeholder for when UI events published by the editor need to be listened for
        eventDispatcher.subscribeToEvent(EditorEvents.WebMessage, (ev) => this.handleWebMessage(ev));
    }
}
