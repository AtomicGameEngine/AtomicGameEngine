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
        this.clientServices = new ClientExtensionServices.WebViewServicesProvider();
        this.clientServices.subscribeToEvents(this);
    }

    private eventDispatcher: Editor.Extensions.EventDispatcher = new ClientExtensionServices.EventDispatcher();

    clientServices: ClientExtensionServices.WebViewServicesProvider;
    loadService(service: Editor.ClientExtensions.ClientEditorService) {
        try {
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
serviceLocator.loadService(new tbExtension());
