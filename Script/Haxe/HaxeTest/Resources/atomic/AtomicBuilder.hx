package atomic;

import haxe.macro.Compiler;
import haxe.macro.ExampleJSGenerator;
import haxe.macro.JSGenApi;
import haxe.macro.Type;
import haxe.macro.Expr;
import sys.FileSystem;
import sys.io.File;

class AtomicBuilder {
	var api: JSGenApi;
	var buf: StringBuf;
	var inits : List<TypedExpr>;
	var statics : List<{ c : ClassType, f : ClassField }>;
	var requirements: Map<String, Array<String>>;
	var currClass:BaseType;
	var isEnum:Bool;
	var components:List<ClassType>;
	var enums:List<EnumType>;
	static var reservedWords:Array<String> = ['Math', 'Array', 'Date', 'Enum', 'Class', 'Dynamic', 'Bool', 'Float', 'Int', 'String', 'Error'];
	
	
	function new(api:JSGenApi) {
		this.api = api;
		this.buf = new StringBuf();
		this.inits = new List();
		this.statics = new List();
		this.requirements = new Map();
		this.components = new List();
		this.enums = new List();
		api.setTypeAccessor(getType);
		build();
	}
	
	function getType( t : Type ) {
		return switch(t) {
			case TInst(c, _):
				getPath(c.get(), 0);
			case TEnum(e, _): 
				getPath(e.get(), 1);
			case TAbstract(a, _): 
				getPath(a.get(), 2);
			default: throw "assert";
		};
	}
	
	//n is a type, 0 - inst, 1 - enum, 2 - abstract
	function getPath(t : BaseType, ?nt: Int) {
		var s:Array<String> = t.module.split(".");
		if (s[0].toLowerCase() == "atomic") {
			if (StringTools.startsWith(t.name, "Atomic")) {
				return t.name;
			}
			return "Atomic." + t.name;
		}
		//skip to do not require itself
		if (t.name == currClass.name) return t.name;
		var mod = t.module.split(".");
		var n = "";
		//it it's enum
		//if it's an enum inside of class
		if (nt == 1) {
			//var i = 0;
			//for (i in 0...(mod.length - 1)) {
			//	n += mod[i] + "/"; 
			//}
			
			addReq("modules/" + t.name);
			return t.name;
		}
		//check if it's a script or a component
		//probably I should rework it, to make components and scripts works with metadata, such like:
		//@:AtomicComponent or @:AtomicScript
		if (mod[0] == "scripts" || mod[0] == "components") {	
			for (pa in mod) {
				if (mod[0] == pa) {
					n += pa;
					continue;
				}
				n += "/" + pa;
			}
			addReq(n);
		} else {
			addReq("modules/" + t.name);
		}
		return t.name;
	}
	
	function addReq(name:String) {
		if (!requirements.exists(currClass.name)) {
			requirements.set(currClass.name, new Array());
		}
		var arr = requirements.get(currClass.name);
		for (i in arr) {
			if (i == name) return;
		}
		arr.push(name);
	}

	
	function build():Void {
		for( t in api.types )
			genType(t);
		for (comp in components) 
			genComponent(comp);
	}
	
	function genType( t : Type ):Void {
		switch( t ) {
		case TInst(c, _):
			var c = c.get();
			if ( c.init != null ) {
				//trace("Adding init: " + c.name);
				inits.add(c.init);
			}
			if ( !c.isExtern ) 
				genClass(c);
		//Enum is not really supported yet
		case TEnum(r, _):
			var e = r.get();
			if( !e.isExtern ) genEnum(e);
		default:
		}
	}
	
	inline function print(str):Void {
		buf.add(str);
	}
	
	inline function prepend(str):Void {
		var b = new StringBuf();
		b.add(str);
		b.add(buf.toString());
		buf = b;
	}

	inline function newline():Void {
		buf.add(";\n");
	}
	
	inline function printExtend__():Void {
		var str:String = "var __extends = (this && this.__extends) || function (d, b) {for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];function __() { this.constructor = d; };__.prototype = b.prototype;d.prototype = new __();};\n";
		print(str);
	}
	
	inline function genExpr(e):Void {
		print(getExpr(e));
	}
	
	inline function getExpr(e):String {
		var out = api.generateValue(e);
		return out;
	}
	
	function checkRequires(c: ClassType):Void {
		var arr = requirements.get(currClass.name);
		if (arr == null || arr.length == 0) return;
		for (req in requirements.get(currClass.name)) {
			var a = req.split("/");
			var name = a[a.length - 1];
			//TODO fix that check
			if (name == currClass.name) continue;
			//exclude reservedWords
			if (reservedWords.indexOf(name) >= 0) continue;
			prepend("var " + name + " = require(\"" + req + "\");\n");
		}
	}
	
	function genClass(c:ClassType):Void {
		if (c.pack.length > 0 && c.pack[0].toLowerCase() == "components") {
			components.add(c);
		} else {
			genScript(c);
		}
	}
	
	function genConstructor(c: ClassType):Void {
		if (c.constructor != null) {
			var constructor = getExpr(c.constructor.get().expr());
			constructor = StringTools.replace(constructor, "function(", 'function ${c.name} (');
			print(constructor);
			newline();
		} else {
			print('function ${c.name} (){}');
			newline();
		}
	}
	
	function genClassBoody(c: ClassType):Void {
		api.setCurrentClass(c);
		currClass = c;
		printExtend__();
		print('var ${c.name} = (function(_super) {\n');
		print('__extends(${c.name}, _super);\n');
		genConstructor(c);
		for( f in c.statics.get() )
			genStaticField(c, f);
		for( f in c.fields.get() ) {
			genClassField(c, f);
		}
		newline();
		print('return ${c.name};\n');
		print("})(" + (c.superClass == null ? "Object" : getPath(c.superClass.t.get())) +");\n");
		print('module.exports = ${c.name};\n');
		checkRequires(c);
	}
	
	function genScript(c: ClassType):Void {
		genClassBoody(c);
		writeFile();
	}
	
	function genComponent(c: ClassType):Void {
		print("\"atomic component\"");
		newline();
		genClassBoody(c);
		writeFile();
	}
	
	function genEnum(e: EnumType) {
		currClass = e;
		isEnum = true;
		var p = getPath(e);
		print('var ${e.name}');
		newline();
		print('(function (${e.name}) {\n');
		var constructs = e.names.map(api.quoteString).join(",");
		for( c in e.constructs.keys() ) {
			var c = e.constructs.get(c);
			var f = c.name;
			print('$p.$f = ');
			switch( c.type ) {
			case TFun(args, _):
				var sargs = args.map(function(a) return a.name).join(",");
				print('function($sargs) { var $$me = ["${c.name}",${c.index},$sargs]; return $$me; };\n');
			default:
				print("[" + api.quoteString(c.name) + "," + c.index + "];\n");
			}
		}
		print('})(${e.name} || (${e.name} = {}));\n');
		print('module.exports = ${e.name};\n');
		writeFile();
	}
	
	function genStaticField(c: ClassType, f: ClassField):Void {
		var field = f.name;
		var e = f.expr();
		if( e == null ) {
			print("null");
			newline();
		} else switch( f.kind ) {
		case FMethod(_):
			print(c.name + '.$field = ');
			genExpr(e);
			if (f.name == "main") {
				newline();
				genExpr(api.main);
			}
			newline();
		case FVar(r, w):
			print(c.name + '.$field = ');
			genExpr(e);
			newline();
		default:
			statics.add( { c : c, f : f } );
		}
	}
	
	function genClassField(c: ClassType, f: ClassField):Void {
		var field = f.name;
		print(c.name + '.prototype.$field = ');
		var e = f.expr();
		if( e == null )
			print("null");
		else {
			genExpr(e);
		}
		newline();
	}
	
	//saves current buffer to file and then clears buffer
	function writeFile():Void {
		var path = "";
		if (currClass.pack.length > 0 && (currClass.pack[0].toLowerCase() == "scripts" || currClass.pack[0].toLowerCase() == "components") && !isEnum) {
			for (p in currClass.pack) {
				path += p + "/";
			}
		} else {
			path = "modules/";
		}
		if (!FileSystem.exists(path)) {
			FileSystem.createDirectory(path);
		}
		path += currClass.name + ".js";
		File.saveContent(path, buf.toString());
		buf = new StringBuf();
		isEnum = false;
	}
	
	static function use() {
		Compiler.setCustomJSGenerator(function(_api) {
			new AtomicBuilder(_api);
		});
	}
	
}