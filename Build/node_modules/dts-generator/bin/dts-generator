#!/usr/bin/env node
var dtsGenerator = require('./dts-generator.js');
/* istanbul ignore if: we use the module interface in testing */
if (!module.parent) {
	dtsGenerator.default(process.argv.slice(2)).then(function (code) {
		return process.exit(code || 0);
	}, function (err) {
		throw err;
	});
}
else {
	module.exports = dtsGenerator;
}
