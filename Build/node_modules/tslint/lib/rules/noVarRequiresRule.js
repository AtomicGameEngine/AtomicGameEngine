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
        var requiresWalker = new NoVarRequiresWalker(sourceFile, this.getOptions());
        return this.applyWithWalker(requiresWalker);
    };
    Rule.FAILURE_STRING = "require statement not part of an import statement";
    return Rule;
}(Lint.Rules.AbstractRule));
exports.Rule = Rule;
var NoVarRequiresWalker = (function (_super) {
    __extends(NoVarRequiresWalker, _super);
    function NoVarRequiresWalker(sourceFile, options) {
        _super.call(this, sourceFile, options);
    }
    NoVarRequiresWalker.prototype.createScope = function () {
        return {};
    };
    NoVarRequiresWalker.prototype.visitCallExpression = function (node) {
        var expression = node.expression;
        if (this.getCurrentDepth() <= 1 && expression.kind === ts.SyntaxKind.Identifier) {
            var identifierName = expression.text;
            if (identifierName === "require") {
                this.addFailure(this.createFailure(node.getStart(), node.getWidth(), Rule.FAILURE_STRING));
            }
        }
        _super.prototype.visitCallExpression.call(this, node);
    };
    return NoVarRequiresWalker;
}(Lint.ScopeAwareRuleWalker));
