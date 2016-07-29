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

import * as HostExtensionServices from "./HostExtensionServices";
import * as EditorUI from "../ui/EditorUI";
import ProjectBasedExtensionLoader from "./coreExtensions/ProjectBasedExtensionLoader";
import TypescriptLanguageExtension from "./languageExtensions/TypscriptLanguageExtension";
import CSharpLanguageExtension from "./languageExtensions/CSharpLanguageExtension";

/**
 * Generic service locator of editor services that may be injected by either a plugin
 * or by the editor itself.
 */
export class ServiceLocatorType implements Editor.HostExtensions.HostServiceLocator {

    constructor() {
        this.resourceServices = new HostExtensionServices.ResourceServicesProvider();
        this.projectServices = new HostExtensionServices.ProjectServicesProvider();
        this.sceneServices = new HostExtensionServices.SceneServicesProvider();
        this.uiServices = new HostExtensionServices.UIServicesProvider();
    }

    private eventDispatcher: Atomic.UIWidget = null;

    resourceServices: HostExtensionServices.ResourceServicesProvider;
    projectServices: HostExtensionServices.ProjectServicesProvider;
    sceneServices: HostExtensionServices.SceneServicesProvider;
    uiServices: HostExtensionServices.UIServicesProvider;

    loadService(service: Editor.HostExtensions.HostEditorService) {
        try {
            service.initialize(this);
        } catch (e) {
            EditorUI.showModalError("Extension Error", `Error detected in extension ${service.name}:\n${e}\n\n ${e.stack}`);
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
        this.sceneServices.subscribeToEvents(this);
        this.uiServices.subscribeToEvents(this);
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
serviceLocator.loadService(new ProjectBasedExtensionLoader());
serviceLocator.loadService(new TypescriptLanguageExtension());
serviceLocator.loadService(new CSharpLanguageExtension());
