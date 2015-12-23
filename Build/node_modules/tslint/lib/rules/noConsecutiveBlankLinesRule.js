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
        return this.applyWithWalker(new NoConsecutiveBlankLinesWalker(sourceFile, this.getOptions()));
    };
    Rule.FAILURE_STRING = "consecutive blank lines are disallowed";
    return Rule;
}(Lint.Rules.AbstractRule));
exports.Rule = Rule;
var NoConsecutiveBlankLinesWalker = (function (_super) {
    __extends(NoConsecutiveBlankLinesWalker, _super);
    function NoConsecutiveBlankLinesWalker() {
        _super.apply(this, arguments);
    }
    NoConsecutiveBlankLinesWalker.prototype.visitSourceFile = function (node) {
        var _this = this;
        _super.prototype.visitSourceFile.call(this, node);
        var newLinesInARowSeenSoFar = 1;
        Lint.scanAllTokens(ts.createScanner(ts.ScriptTarget.ES5, false, ts.LanguageVariant.Standard, node.text), function (scanner) {
            var startPos = scanner.getStartPos();
            if (_this.tokensToSkipStartEndMap[startPos] != null) {
                scanner.setTextPos(_this.tokensToSkipStartEndMap[startPos]);
                newLinesInARowSeenSoFar = 0;
                return;
            }
            if (scanner.getToken() === ts.SyntaxKind.NewLineTrivia) {
                newLinesInARowSeenSoFar += 1;
                if (newLinesInARowSeenSoFar >= 3) {
                    var failure = _this.createFailure(scanner.getStartPos(), 1, Rule.FAILURE_STRING);
                    _this.addFailure(failure);
                }
            }
            else {
                newLinesInARowSeenSoFar = 0;
            }
        });
    };
    return NoConsecutiveBlankLinesWalker;
}(Lint.SkippableTokenAwareRuleWalker));
