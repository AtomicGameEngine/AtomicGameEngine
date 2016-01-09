"use strict";
var __extends = (this && this.__extends) || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
};
var Lint = require("../lint");
var Rule = (function (_super) {
    __extends(Rule, _super);
    function Rule() {
        _super.apply(this, arguments);
    }
    Rule.prototype.apply = function (sourceFile) {
        return this.applyWithWalker(new NoConstructorVarsWalker(sourceFile, this.getOptions()));
    };
    Rule.FAILURE_STRING_PART = " cannot be declared in the constructor";
    return Rule;
}(Lint.Rules.AbstractRule));
exports.Rule = Rule;
var NoConstructorVarsWalker = (function (_super) {
    __extends(NoConstructorVarsWalker, _super);
    function NoConstructorVarsWalker() {
        _super.apply(this, arguments);
    }
    NoConstructorVarsWalker.prototype.visitConstructorDeclaration = function (node) {
        var parameters = node.parameters;
        for (var _i = 0, parameters_1 = parameters; _i < parameters_1.length; _i++) {
            var parameter = parameters_1[_i];
            if (parameter.modifiers != null && parameter.modifiers.length > 0) {
                var name_1 = parameter.name;
                var errorMessage = "'" + name_1.text + "'" + Rule.FAILURE_STRING_PART;
                var lastModifier = parameter.modifiers[parameter.modifiers.length - 1];
                var position = lastModifier.getEnd() - parameter.getStart();
                this.addFailure(this.createFailure(parameter.getStart(), position, errorMessage));
            }
        }
        _super.prototype.visitConstructorDeclaration.call(this, node);
    };
    return NoConstructorVarsWalker;
}(Lint.RuleWalker));
exports.NoConstructorVarsWalker = NoConstructorVarsWalker;
