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

export interface Breakpoint {
    fileName: string;
    lineNumber: number;
}

class BreakpointList {
    // list of breakpoints
    private breakpoints: Breakpoint[] = [];


    private indexOfBreakpoint(fileName: string, lineNumber: number): number {

        let curr: Breakpoint;
        for (let i = 0, iEnd = this.breakpoints.length; i < iEnd; i++) {
            curr = this.breakpoints[i];
            if (curr.fileName == fileName && curr.lineNumber == lineNumber) {
                return i;
            }
        }
        return -1;
    }

    removeAllBreakpoints() {
        this.breakpoints.length = 0;
    }

    removeBreakpoint(fileName: string, lineNumber: number) {
        const idx = this.indexOfBreakpoint(fileName, lineNumber);
        if (idx != -1) {
            this.breakpoints.splice(idx);
        }
    }

    addBreakpoint(fileName: string, lineNumber: number) {
        const idx = this.indexOfBreakpoint(fileName, lineNumber);
        if (idx == -1) {
            console.log("Adding breakpoint: " + fileName + ":" + lineNumber);
            this.breakpoints.push({
                fileName,
                lineNumber
            });
        }
    }

    getBreakpoint(fileName: string, lineNumber: number): Breakpoint {
        const idx = this.indexOfBreakpoint(fileName, lineNumber);
        return this.breakpoints[idx];
    }

    toggleBreakpoint(fileName: string, lineNumber: number) {
        let decoration = this.getBreakpoint(fileName, lineNumber);
        if (decoration) {
            this.removeBreakpoint(fileName, lineNumber);
        } else {
            this.addBreakpoint(fileName, lineNumber);
        }
    }

    getBreakpoints(): Breakpoint[] {
        return this.breakpoints;
    }
}

/**
 * extension that will communicate with the duktape debugger
 */
export default class DuktapeDebuggerExtension extends Atomic.ScriptObject implements Editor.HostExtensions.HostEditorService {
    name: string = "HostDebuggerExtension";
    description: string = "This service supports the duktape debugger interface";

    private debuggerWebClient: WebView.WebClient;
    private clientProxyMappings: {
        toggleBreakpoint: string,
        addBreakpoint: string,
        removeBreakpoint: string
    };

    private serviceRegistry: Editor.HostExtensions.HostServiceLocator = null;

    private breakpointList = new BreakpointList();

    /**
     * Inject this language service into the registry
     * @return {[type]}             True if successful
     */
    initialize(serviceLocator: Editor.HostExtensions.HostServiceLocator) {
        // We care about both resource events as well as project events
        serviceLocator.resourceServices.register(this);
        serviceLocator.projectServices.register(this);
        serviceLocator.uiServices.register(this);
        this.serviceRegistry = serviceLocator;
    }

    /**
     * Handle messages that are submitted via Atomic.Query from within a web view editor.
     * @param message The message type that was submitted to be used to determine what the data contains if present
     * @param data any additional data that needs to be submitted with the message
     */
    handleWebMessage(webMessage: WebView.WebMessageEvent, messageType: string, data: any) {
        switch (messageType) {
            case "Debugger.AddBreakpoint":
                this.addBreakpoint(data);
                this.webMessageEventResponse(webMessage);
                break;
            case "Debugger.RemoveBreakpoint":
                this.removeBreakpoint(data);
                this.webMessageEventResponse(webMessage);
                break;
            case "Debugger.ToggleBreakpoint":
                this.toggleBreakpoint(data);
                this.webMessageEventResponse(webMessage);
                break;
            case "Debugger.RemoveAllBreakpoints":
                this.breakpointList.removeAllBreakpoints();
                this.webMessageEventResponse(webMessage);
                break;
            case "Debugger.GetBreakpoints":
                this.webMessageEventResponse(webMessage, this.breakpointList.getBreakpoints());
                break;
            case "Debugger.RegisterDebuggerFrame":
                this.registerDebuggerFrame(webMessage, data);
                this.webMessageEventResponse(webMessage);
                break;
        }
    }

    webMessageEventResponse<T>(originalMessage: WebView.WebMessageEvent, response?: T) {
        if (response) {
            const wrappedResponse: WebView.WebMessageEventResponse<T> = {
                response
            };

            originalMessage.handler.success(JSON.stringify(wrappedResponse));
        } else {
            originalMessage.handler.success();
        }
    }

    registerDebuggerFrame(originalMessage: WebView.WebMessageEvent, mappings) {
        this.debuggerWebClient = originalMessage.handler.webClient;
        this.clientProxyMappings = mappings;
    }

    addBreakpoint(bp: Breakpoint) {
        this.breakpointList.addBreakpoint(bp.fileName, bp.lineNumber);
        if (this.debuggerWebClient) {
            this.proxyWebClientMethod(
                this.debuggerWebClient,
                this.clientProxyMappings.addBreakpoint,
                bp.fileName,
                bp.lineNumber,
                false);
        }
    }

    removeBreakpoint(bp: Breakpoint) {
        this.breakpointList.removeBreakpoint(bp.fileName, bp.lineNumber);
        if (this.debuggerWebClient) {
            this.proxyWebClientMethod(
                this.debuggerWebClient,
                this.clientProxyMappings.removeBreakpoint,
                bp.fileName,
                bp.lineNumber,
                false);
        }
    }

    toggleBreakpoint(bp: Breakpoint) {
        this.breakpointList.toggleBreakpoint(bp.fileName, bp.lineNumber);
        if (this.debuggerWebClient) {
            this.proxyWebClientMethod(
                this.debuggerWebClient,
                this.clientProxyMappings.toggleBreakpoint,
                bp.fileName,
                bp.lineNumber,
                false);
        }
    }


    resume() {

    }

    attach() {

    }

    detach() {

    }

    /**
     * Utility function that will compose a method call to the web client and execute it
     * It will construct it in the form of "MethodName(....);"
     * @param  {WebView.WebClient} webClient
     * @param  {string} methodName
     * @param  {any} ...params
     */
    proxyWebClientMethod(webClient: WebView.WebClient, methodName: string, ...params) {
        let paramBuilder = [];

        for (let p of params) {
            switch (typeof (p)) {
                case "boolean":
                case "number":
                    paramBuilder.push(p.toString());
                    break;
                default:
                    paramBuilder.push(`"${p}"`);
                    break;
            }
        }

        const methodCall = `${methodName}(${paramBuilder.join(",")});`;
        webClient.executeJavaScript(methodCall);
    }
}
