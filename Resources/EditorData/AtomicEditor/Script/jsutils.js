__atomic_acorn = require('./acorn');
__atomic_beautify = require('./beautify');
__atomic_sourcemap = require('./source-map');

exports.parseToJSON = function (source) {

	var start = new Date().getTime();

	var comments = [];

	var ast = __atomic_acorn.parse( source, {
    	// collect ranges for each node
    	ranges: true,
    	locations: true,
    	onComment: comments,
	});

	var end = new Date().getTime();
	var time = end - start;

	// print('Acorn parse time: ' + time);

	ast["comments"] = comments;

	start = new Date().getTime();
	ast = JSON.stringify(ast);
	end = new Date().getTime();
	time = end - start;

	// print('JSON.stringify time: ' + time);

	return ast;

}


exports.parseErrorCheck = function(source) {


	try {
		__atomic_acorn.parse( source, {
	    	ranges: true,
	    	locations: true,
		});

	} catch(e) {

        //if (!(e instanceof __atomic_acorn.SyntaxError)) throw e;

        if (!(e instanceof SyntaxError))
        	return false; // rethrow?

        return e;

    }


	return false;

}

// TODO: options
exports.jsBeautify = function (source) {

	return __atomic_beautify.js_beautify(source);

}

exports.getRealLineNumber = function (map, line) {
    var jsonMap = JSON.parse(map);
    var smc = new __atomic_sourcemap.SourceMapConsumer(jsonMap);
    var pos = smc.originalPositionFor({line: line, column: 100000});
    return pos.line;
}