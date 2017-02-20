// Duktape built-ins

// extracted from lib.d.ts
declare interface Console {
    log(message?: any, ...optionalParams: any[]): void;
}

declare var console: Console;

// Duktape require isn't recognized as a function, but can be used as one
declare function require(filename: string): any;

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
