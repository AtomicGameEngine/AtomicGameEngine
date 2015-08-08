package atomic;

import haxe.macro.Compiler;
import haxe.macro.Context;
import haxe.macro.ExampleJSGenerator;
import haxe.macro.JSGenApi;
import haxe.macro.PositionTools;
import haxe.macro.Type;
import haxe.macro.Expr;
import sys.io.File;

class AtomicBuilder {
	var api: JSGenApi;
	var buf: StringBuf;
	var inits : List<TypedExpr>;
	var statics : List<{ c : ClassType, f : ClassField }>;
	var requirements: List<String>;
	
	function new(api:JSGenApi) {
		this.api = api;
		this.buf = new StringBuf();
		this.inits = new List();
		this.statics = new List();
		this.requirements = new List();
		build();
	}
	
	function build():Void {
		for( t in api.types )
			genType(t);
	}
	
	function genType( t : Type ):Void {
		switch( t ) {
		case TInst(c, _):
			var c = c.get();
			if( c.init != null )
				inits.add(c.init);
			if( !c.isExtern ) genClass(c);
		//Enum is not really supported yet
		case TEnum(r, _):
			var e = r.get();
			//if( !e.isExtern ) genEnum(e);
		default:
		}
	}
	
	inline function print(str):Void {
		buf.add(str);
	}

	inline function newline():Void {
		buf.add(";\n");
	}
	
	//extend function which I took from typescript
	inline function printExtend__():Void {
		var str:String = "var __extends = (this && this.__extends) || function (d, b) {for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];function __() { this.constructor = d; };__.prototype = b.prototype;d.prototype = new __();};\n";
		print(str);
	}
	
	inline function genExpr(e):Void {
		print(getExpr(e));
	}
	
	inline function getExpr(e):String {
		var out = api.generateValue(e);
		//TODO: properly handle components
		//I'm renaming it, to avoid components_Star e.g.
		out = StringTools.replace(out, "atomic.", "");
		out = StringTools.replace(out, "scripts_", "");
		out = StringTools.replace(out, "components_", "");
		return out;
	}
	
	//TODO: Rethink how to handle imports correctly, because now it's really not a good way
	//https://github.com/HaxeFoundation/haxe/issues/3560
	function checkRequires(c: ClassType):Void {
		requirements.clear();
		var mets = c.meta.get();
		
		for (meta in mets) {
			if (meta.name == ":require") {
				for (param in meta.params) {
					switch(param.expr) {
						case EConst(CString(s)):
							requirements.add(s);
						default:
					}
				}
			}
		}
		for (req in requirements) {
			var name = "";
			if (req.charAt(0) == 'C') {
				name = "Components/" + req.substr(1, req.length);
			}else if (req.charAt(0) == 'S') {
				name = "Scripts/" + req.substr(1, req.length);		
			} else if (req.charAt(0) == 'M') {
				name = "Modules/" + req.substr(1, req.length);
			}
			print("var " + req.substr(1, req.length) + " = require(\"" + name + "\");\n");
		}
	}
	
	function genClass(c:ClassType):Void {
		if (c.name == "Std") return;
		if (c.pack[0] == "components") {
			genComponent(c);
		} else {
			genScript(c);
		}
	}
	
	function genConstructor(c: ClassType):Void {
		if (c.constructor != null) {
			var constructor = getExpr(c.constructor.get().expr());
			constructor = StringTools.replace(constructor, "function(", "function " + c.name + "(");
			print(constructor);
			newline();
		} else {
			print("function " + c.name + "(){}");
			newline();
		}
	}
	
	function genScript(c: ClassType):Void {
		//If pack length equals 0, so it's probably a haxe's class, so, require a HxOverrides
		if (c.pack.length == 0) {
			print("var HxOverrides = require(\"./HxOverrides\");\n");
		}
		api.setCurrentClass(c);
		checkRequires(c);
		printExtend__();
		print("var " + c.name + " = (function(_super) {\n");
		print("__extends(" + c.name + ", _super);\n");
		genConstructor(c);
		for( f in c.statics.get() )
			genStaticField(c, f);
		for( f in c.fields.get() ) {
			genClassField(c, f);
		}
		newline();
		print("return " + c.name + ";\n");
		print("})(" + (c.superClass == null ? "Object" : c.superClass.t.toString()) +");\n");
		print("module.exports = " + c.name + ";");
		
		var p = c.pack[0] + ("/") + c.name + ".js";
		//If it's a haxe's class
		p = StringTools.replace(p, "null", "Modules");
		File.saveContent(p, buf.toString());
		buf = new StringBuf();
	}
	
	function genComponent(c: ClassType):Void {
		api.setCurrentClass(c);		
		checkRequires(c);
		printExtend__();
		print("\"atomic component\"");
		newline();
		print("var " + c.name + " = (function(_super) {\n");
		print("__extends(" + c.name + ", _super);\n");
		genConstructor(c);
		for( f in c.statics.get() )
			genStaticField(c, f);
		for( f in c.fields.get() ) {
			genClassField(c, f);
		}
		newline();
		print("return " + c.name + ";\n");
		print("})(" + (c.superClass == null ? "Object" : StringTools.replace(c.superClass.t.toString(), "components.", "")) +");\n");
		print("module.exports = " + c.name + ";");
		var p = c.pack[0] + ("/") + c.name + ".js";
		File.saveContent(p, buf.toString());
		buf = new StringBuf();
	}
	
	function genStaticField(c: ClassType, f: ClassField):Void {
		var field = f.name;
		var e = f.expr();
		if( e == null ) {
			print('NULL :(');
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
	
	static function use() {
		Compiler.setCustomJSGenerator(function(_api) {
			new AtomicBuilder(_api);
		});
	}
	
}