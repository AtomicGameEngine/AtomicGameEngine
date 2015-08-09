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
	var requirements: Map<String, Array<String>>;
	var currClass:ClassType;
	var components:List<ClassType>;
	
	function new(api:JSGenApi) {
		this.api = api;
		this.buf = new StringBuf();
		this.inits = new List();
		this.statics = new List();
		this.requirements = new Map();
		this.components = new List();
		api.setTypeAccessor(getType);
		build();
	}
	
	function getType( t : Type ) {
		return switch(t) {
			case TInst(c, _):
				getPath(c.get());
			case TEnum(e, _): 
				getPath(e.get());
			case TAbstract(a, _): 
				getPath(a.get());
			default: throw "assert";
		};
	}
	
	function getPath( t : BaseType ) {
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
		if (mod[0].toLowerCase() == "scripts") {
			var n = "";
			for (pa in mod) {
				if (mod[0] == pa) {
					n += pa;
					continue;
				}
				n += "/" + pa;
			}
			addReq(n);
		} else if (mod[0].toLowerCase() == "components") {
			var n = "";
			for (pa in mod) {
				if (mod[0] == pa) {
					n += pa;
					continue;
				}
				n += "/" + pa;
			}
			addReq(n);
		} else {
			addReq("Modules/" + t.name);
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
	
	//https://github.com/HaxeFoundation/haxe/issues/3560
	function checkRequires(c: ClassType):Void {
		var arr = requirements.get(currClass.name);
		if (arr == null || arr.length == 0) return;
		for (req in requirements.get(currClass.name)) {
			var a = req.split("/");
			var name = a[a.length - 1];
			prepend("var " + name + " = require(\"" + req + "\");\n");
		}
	}
	
	function genClass(c:ClassType):Void {
		//if (c.name == "Std") return;
		if (c.pack.length > 0 && c.pack[0].toLowerCase() == "components") {
			components.add(c);
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
		//if (c.pack.length == 0) {
		//	print("var HxOverrides = require(\"./HxOverrides\");\n");
		//}
		api.setCurrentClass(c);
		currClass = c;
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
		print("})(" + (c.superClass == null ? "Object" : getPath(c.superClass.t.get())) +");\n");
		print("module.exports = " + c.name + ";\n");
		checkRequires(c);
		var p = "";
		//script path
		if (c.pack.length > 0 && c.pack[0].toLowerCase() == "scripts") {
			for (pa in c.pack) {
				p += pa + "/";
			}
			p += c.name + ".js";
		} else {
			p = "Modules/" + c.name + ".js";
		}
		File.saveContent(p, buf.toString());
		buf = new StringBuf();
	}
	
	function genComponent(c: ClassType):Void {
		api.setCurrentClass(c);
		currClass = c;
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
		print("})(" + (c.superClass == null ? "Object" : getPath(c.superClass.t.get())) +");\n");
		print("module.exports = " + getPath(c) + ";\n");
		checkRequires(c);
		var p = "";
		for (pa in c.pack) {
			p += pa + "/";
		}
		p += c.name + ".js";
		File.saveContent(p, buf.toString());
		buf = new StringBuf();
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
	
	static function use() {
		Compiler.setCustomJSGenerator(function(_api) {
			new AtomicBuilder(_api);
		});
	}
	
}