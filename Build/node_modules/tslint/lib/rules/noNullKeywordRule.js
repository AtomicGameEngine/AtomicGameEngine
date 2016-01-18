"use strict";
var __extends = (this && this.__extends) || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
};
var ts = require("typescript");
var Lint = require("../lint");
var Rule = (function (_super) {
    __extends(Rule, _super);
    function Rule() {
        _super.apply(this, arguments);
    }
    Rule.prototype.apply = function (sourceFile) {
        return this.applyWithWalker(new NullWalker(sourceFile, this.getOptions()));
    };
    Rule.FAILURE_STRING = "Use 'undefined' instead of 'null'";
    return Rule;
}(Lint.Rules.AbstractRule));
exports.Rule = Rule;
var NullWalker = (function (_super) {
    __extends(NullWalker, _super);
    function NullWalker() {
        _super.apply(this, arguments);
    }
    NullWalker.prototype.visitNode = function (node) {
        _super.prototype.visitNode.call(this, node);
        if (node.kind === ts.SyntaxKind.NullKeyword) {
            this.addFailure(this.createFailure(node.getStart(), node.getWidth(), Rule.FAILURE_STRING));
        }
    };
    return NullWalker;
}(Lint.RuleWalker));
