(function (factory) {
    if (typeof module === 'object' && typeof module.exports === 'object') {
        var v = factory(require, exports); if (v !== undefined) module.exports = v;
    }
    else if (typeof define === 'function' && define.amd) {
        define(["require", "exports", 'intern!object', 'intern/chai!assert', 'intern/dojo/node!../../../bin/dts-generator'], factory);
    }
})(function (require, exports) {
    "use strict";
    var registerSuite = require('intern!object');
    var assert = require('intern/chai!assert');
    var dtsGenerator = require('intern/dojo/node!../../../bin/dts-generator');
    registerSuite({
        name: 'bin/dts-generator',
        api: function () {
            assert.isFunction(dtsGenerator, 'dtsGenerator should be a function');
            assert.strictEqual(Object.keys(dtsGenerator).length, 0, 'There should be no other keys');
        },
        basic: function () {
            return dtsGenerator([
                '-name',
                'foo',
                '-project',
                'tests/support/foo',
                '-out',
                'tmp/foo.cli.d.ts'
            ]);
        }
    });
});
