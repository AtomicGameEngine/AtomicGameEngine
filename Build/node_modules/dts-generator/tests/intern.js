(function (factory) {
    if (typeof module === 'object' && typeof module.exports === 'object') {
        var v = factory(require, exports); if (v !== undefined) module.exports = v;
    }
    else if (typeof define === 'function' && define.amd) {
        define(["require", "exports"], factory);
    }
})(function (require, exports) {
    "use strict";
    exports.loaderOptions = {
        packages: [
            { name: 'dts-generator', location: '.' }
        ]
    };
    exports.suites = ['dts-generator/tests/unit/all'];
    exports.excludeInstrumentation = /^(?:tests|node_modules)\//;
});
