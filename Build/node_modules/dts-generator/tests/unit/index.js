(function (factory) {
    if (typeof module === 'object' && typeof module.exports === 'object') {
        var v = factory(require, exports); if (v !== undefined) module.exports = v;
    }
    else if (typeof define === 'function' && define.amd) {
        define(["require", "exports", 'intern!object', 'intern/chai!assert', 'intern/dojo/node!../../index', 'intern/dojo/node!fs'], factory);
    }
})(function (require, exports) {
    "use strict";
    var registerSuite = require('intern!object');
    var assert = require('intern/chai!assert');
    var index_1 = require('intern/dojo/node!../../index');
    var fs = require('intern/dojo/node!fs');
    registerSuite({
        name: 'index',
        'api': function () {
            assert.isFunction(index_1.default, 'generate should be a function');
        },
        'generate': function () {
            return index_1.default({
                name: 'foo',
                baseDir: 'tests/support/foo',
                files: ['index.ts'],
                out: 'tmp/foo.d.ts'
            }).then(function () {
                var contents = fs.readFileSync('tmp/foo.d.ts', { encoding: 'utf8' });
                assert(contents, 'foo.d.ts should exist and have contents');
                assert.include(contents, "module 'foo/index'");
                assert.include(contents, "module 'foo/Bar'");
            });
        },
        'no files': function () {
            return index_1.default({
                name: 'foo',
                baseDir: 'tests/support/foo',
                out: 'tmp/foo.nofiles.d.ts'
            }).then(function () {
                var contents = fs.readFileSync('tmp/foo.nofiles.d.ts', { encoding: 'utf8' });
                assert(contents, 'foo.nofiles.d.ts should exist and have contents');
                assert.include(contents, "module 'foo/index'");
                assert.include(contents, "module 'foo/Bar'");
            });
        },
        'project': function () {
            return index_1.default({
                name: 'foo',
                project: 'tests/support/foo',
                out: 'tmp/foo.config.d.ts'
            }).then(function () {
                var contents = fs.readFileSync('tmp/foo.config.d.ts', { encoding: 'utf8' });
                assert(contents, 'foo.config.d.ts should exist and have contents');
                assert.include(contents, "module 'foo/index'");
                assert.include(contents, "module 'foo/Bar'");
            });
        },
        'es6 main module': function () {
            return index_1.default({
                name: 'foo',
                project: 'tests/support/foo-es6',
                out: 'tmp/foo.es6.d.ts',
                main: 'index.ts'
            }).then(function () {
                var contents = fs.readFileSync('tmp/foo.es6.d.ts', { encoding: 'utf8' });
                assert(contents, 'foo.es6.d.ts should exist and have contents');
                // assert.include(contents, `module 'foo/index'`);
                // assert.include(contents, `module 'foo/Bar'`);
            });
        }
    });
});
