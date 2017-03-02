import HostInteropType from "../interop";

export interface Breakpoint {
    fileName: string;
    lineNumber: number;
}

/**
 * Get a list of all breakpoints that have been registered
 * @return {Breakpoint[]}
 */
export async function getBreakpoints() : Promise<Breakpoint[]> {
    return atomicHostRequest<Breakpoint[]>("Debugger.GetBreakpoints");
}

/**
 * Add a single breakpoint
 * @param  {string} fileName
 * @param  {number} lineNumber
 */
export async function addBreakpoint(fileName: string, lineNumber: number) {
    return atomicHostEvent("Debugger.AddBreakpoint", {
        fileName,
        lineNumber
    });
}

/**
 * Remove a single breakpoint
 * @param  {string} fileName
 * @param  {number} lineNumber
 */
export async function removeBreakpoint(fileName: string, lineNumber: number) {
    return atomicHostEvent("Debugger.RemoveBreakpoint", {
        fileName,
        lineNumber
    });
}

/**
 * Toggle a single breakpoint
 * @param  {string} fileName
 * @param  {number} lineNumber
 */
export async function toggleBreakpoint(fileName: string, lineNumber: number) {
    return atomicHostEvent("Debugger.ToggleBreakpoint", {
        fileName,
        lineNumber
    });
}

/**
 * Remove all breakpoints
 */
export async function removeAllBreakpoints() {
    return atomicHostEvent("Debugger.RemoveAllBreakpoints");
}

export const debuggerHostKeys = {
    toggleBreakpoint: "HOST_DEBUGGER_ToggleBreakpoint",
    addBreakpoint: "HOST_DEBUGGER_AddBreakpoint",
    removeBreakpoint: "HOST_DEBUGGER_RemoveBreakpoint"
};

/**
 * Register host-callable commands with the web view.  This registers the
 * this named web view with the host so that it can be notified of any
 * host initiated debugger messages
 * @param {string} name The name associated with the listener.  Ususally the filename.
 */
export function  registerDebuggerListener(name: string) {
    atomicHostEvent("Debugger.RegisterDebuggerListener", {
        name,
        callbacks: debuggerHostKeys
    });
}
