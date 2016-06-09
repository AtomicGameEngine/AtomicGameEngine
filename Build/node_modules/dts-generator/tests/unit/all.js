(function (factory) {
    if (typeof module === 'object' && typeof module.exports === 'object') {
        var v = factory(require, exports); if (v !== undefined) module.exports = v;
    }
    else if (typeof define === 'function' && define.amd) {
        define(["require", "exports", './bin/dts-generator', './index'], factory);
    }
})(function (require, exports) {
    "use strict";
    require('./bin/dts-generator');
    require('./index');
});
