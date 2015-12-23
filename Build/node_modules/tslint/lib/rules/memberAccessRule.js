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
        return this.applyWithWalker(new MemberAccessWalker(sourceFile, this.getOptions()));
    };
    Rule.FAILURE_STRING = "default access modifier on member/method not allowed";
    return Rule;
}(Lint.Rules.AbstractRule));
exports.Rule = Rule;
var MemberAccessWalker = (function (_super) {
    __extends(MemberAccessWalker, _super);
    function MemberAccessWalker(sourceFile, options) {
        _super.call(this, sourceFile, options);
    }
    MemberAccessWalker.prototype.visitConstructorDeclaration = function (node) {
        if (this.hasOption("check-constructor")) {
            this.validateVisibilityModifiers(node);
        }
        _super.prototype.visitConstructorDeclaration.call(this, node);
    };
    MemberAccessWalker.prototype.visitMethodDeclaration = function (node) {
        this.validateVisibilityModifiers(node);
        _super.prototype.visitMethodDeclaration.call(this, node);
    };
    MemberAccessWalker.prototype.visitPropertyDeclaration = function (node) {
        this.validateVisibilityModifiers(node);
        _super.prototype.visitPropertyDeclaration.call(this, node);
    };
    MemberAccessWalker.prototype.visitGetAccessor = function (node) {
        if (this.hasOption("check-accessor")) {
            this.validateVisibilityModifiers(node);
        }
        _super.prototype.visitGetAccessor.call(this, node);
    };
    MemberAccessWalker.prototype.visitSetAccessor = function (node) {
        if (this.hasOption("check-accessor")) {
            this.validateVisibilityModifiers(node);
        }
        _super.prototype.visitSetAccessor.call(this, node);
    };
    MemberAccessWalker.prototype.validateVisibilityModifiers = function (node) {
        if (node.parent.kind === ts.SyntaxKind.ObjectLiteralExpression) {
            return;
        }
        var hasAnyVisibilityModifiers = Lint.hasModifier(node.modifiers, ts.SyntaxKind.PublicKeyword, ts.SyntaxKind.PrivateKeyword, ts.SyntaxKind.ProtectedKeyword);
        if (!hasAnyVisibilityModifiers) {
            this.addFailure(this.createFailure(node.getStart(), node.getWidth(), Rule.FAILURE_STRING));
        }
    };
    return MemberAccessWalker;
}(Lint.RuleWalker));
exports.MemberAccessWalker = MemberAccessWalker;
