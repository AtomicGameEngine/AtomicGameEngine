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
import HostInteropType from "../interop";
import * as ClientExtensionServices from "./ClientExtensionServices";

// Initialize and configure the extensions
import tsExtension from "./languageExtensions/typescript/TypescriptLanguageExtension";
import jsExtension from "./languageExtensions/javascript/JavascriptLanguageExtension";
import tbExtension from "./languageExtensions/turbobadger/TurboBadgerLanguageExtension";

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
    private userPreferences = {};
    
    /**
     * Sets the preferences for the service locator
     * @param  {any} prefs
     * @return {[type]}
     */
    setPreferences(prefs : any) {
        this.userPreferences = prefs;
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

    /** Methods available to extensions **/

    /**
     * Returns the Host Interop module
     * @return {Editor.ClientExtensions.HostInterop}
     */
    getHostInterop(): Editor.ClientExtensions.HostInterop {
        return HostInteropType.getInstance();
    }


    /**
     * Return a preference value or the provided default from the user settings file
     * @param  {string} extensionName name of the extension the preference lives under
     * @param  {string} preferenceName name of the preference to retrieve
     * @param  {number | boolean | string} defaultValue value to return if pref doesn't exist
     * @return {number|boolean|string}
     */
    getUserPreference(extensionName: string, preferenceName: string, defaultValue?: number | boolean | string): number | boolean | string {
        if (this.userPreferences) {
            let extensionPrefs = this.userPreferences["extensions"];
            if (extensionPrefs && extensionPrefs[extensionName]) {
                return extensionPrefs[extensionName][preferenceName] || defaultValue;
            }
        }

        // if all else fails
        return defaultValue;
    }
}

// Singleton service locator that can be referenced
const serviceLocator = new ClientServiceLocatorType();
export default serviceLocator;

// Load up all the internal services
serviceLocator.loadService(new tsExtension());
serviceLocator.loadService(new jsExtension());
serviceLocator.loadService(new tbExtension());
