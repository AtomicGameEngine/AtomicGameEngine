/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License.
 *--------------------------------------------------------------------------------------------*/

/** From Microsoft */

export class Kind {
    public static unknown: string = "";
    public static keyword: string = "keyword";
    public static script: string = "script";
    public static module: string = "module";
    public static class: string = "class";
    public static interface: string = "interface";
    public static type: string = "type";
    public static enum: string = "enum";
    public static variable: string = "var";
    public static localVariable: string = "local var";
    public static function: string = "function";
    public static localFunction: string = "local function";
    public static memberFunction: string = "method";
    public static memberGetAccessor: string = "getter";
    public static memberSetAccessor: string = "setter";
    public static memberVariable: string = "property";
    public static constructorImplementation: string = "constructor";
    public static callSignature: string = "call";
    public static indexSignature: string = "index";
    public static constructSignature: string = "construct";
    public static parameter: string = "parameter";
    public static typeParameter: string = "type parameter";
    public static primitiveType: string = "primitive type";
    public static label: string = "label";
    public static alias: string = "alias";
    public static const: string = "const";
    public static let: string = "let";
    public static warning: string = "warning";

    public static convertKind(kind: string): monaco.languages.CompletionItemKind {
        switch (kind) {
            case Kind.primitiveType:
            case Kind.keyword:
                return monaco.languages.CompletionItemKind.Keyword;
            case Kind.variable:
            case Kind.localVariable:
                return monaco.languages.CompletionItemKind.Variable;
            case Kind.memberVariable:
            case Kind.memberGetAccessor:
            case Kind.memberSetAccessor:
                return monaco.languages.CompletionItemKind.Field;
            case Kind.function:
            case Kind.memberFunction:
            case Kind.constructSignature:
            case Kind.callSignature:
            case Kind.indexSignature:
                return monaco.languages.CompletionItemKind.Function;
            case Kind.enum:
                return monaco.languages.CompletionItemKind.Enum;
            case Kind.module:
                return monaco.languages.CompletionItemKind.Module;
            case Kind.class:
                return monaco.languages.CompletionItemKind.Class;
            case Kind.interface:
                return monaco.languages.CompletionItemKind.Interface;
            case Kind.warning:
                return monaco.languages.CompletionItemKind.File;
        }

        return monaco.languages.CompletionItemKind.Property;
    }
}
