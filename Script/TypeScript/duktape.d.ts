// Duktape built-ins

// extracted from lib.d.ts
declare interface Console {
    log(message?: any, ...optionalParams: any[]): void;
}

declare var console: Console;

// Duktape require isn't recognized as a function, but can be used as one
declare function require(filename: string): any;

/*
 *  Timer API.  These routines are available if you ```import "AtomicEventLoop"```
 */

/**
 * schedules a function to be called on the next frame.  Must require("AtomicEventLoop") in main.js to use.
 * Muat require("AtomicEventLoop") or import "AtomicEventLoop" in main.js to use.
 * @method
 * @param {function} func the Function to call
 * @returns {number} the id of the callback to be used in cancelAnimationFrame in order to cancel the call
 */
declare function requestAnimationFrame(step: any): number;

/**
 * stops a previously queued call to requestAnimationFrame
 * Muat require("AtomicEventLoop") or import "AtomicEventLoop" in main.js to use.
 * @method
 * @param {number} call_id the id of the timer that was created via requestAnimationFrame
 */
declare function cancelAnimationFrame(call_id: number);

/**
 * schedules a function to be called after a certain number of milliseconds
 * Muat require("AtomicEventLoop") or import "AtomicEventLoop" in main.js to use.
 * @method
 * @param {function} func the Function to call
 * @param {number} delay the number of milliseconds
 * @param {any} [args] A comma separated list of parameters to pass to func
 * @returns {number} the id of the timer to be used in clearTimeout in order to cancel the timeout
 */
declare function setTimeout(func: number, delay: number, ...args): number;

/**
 * stops a previously queued timeout.
 * Muat require("AtomicEventLoop") or import "AtomicEventLoop" in main.js to use.
 * @method
 * @param {number} timer_id the id of the timer that was created via setTimeout
 */
declare function clearTimeout(timer_id: number);

/**
 * schedules a function to be called after the end of the current update cycle.  SetImmediate timers are processed FIFO
 * Muat require("AtomicEventLoop") or import "AtomicEventLoop" in main.js to use.
 * @method
 * @param {function} func the Function to call
 * @param {any} [args] A comma separated list of parameters to pass to func
 * @returns {number} the id of the setImmediate function to be used in clearImmediate in order to cancel it.
 */
declare function setImmediate(func: any, ...args): number;

/**
 * stops a previously queued setImmediate callback.
 * Muat require("AtomicEventLoop") or import "AtomicEventLoop" in main.js to use.
 * @method
 * @param {number} timer_id the id of the timer that was created via setImmediate
 */
declare function clearImmediate(timer_id: number);

declare interface DuktapeModule {
    /**
     * List of modules that have been loaded via the "require" statement
     * 
     * @type {string[]}
     * @memberOf DuktapeModule
     */
    modLoaded: string[];
    modSearch(id: string, require, exports, module);
}

declare var Duktape: DuktapeModule;
