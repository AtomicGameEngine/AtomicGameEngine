__atomic_acorn = require('acorn');
__atomic_acorn_loose = require('acorn_loose');
__atomic_beautify = require('beautify');

function __atomic_parse_to_json(source) {

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

function __atomic_parse_to_json_loose(source) {

	var start = new Date().getTime();
		
	var comments = [];

	var ast = __atomic_acorn_loose.parse_dammit( source, {
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


function __atomic_parse_error_check(source) {
		
	
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
function __atomic_js_beautify(source) {

	return __atomic_beautify.js_beautify(source);

}
