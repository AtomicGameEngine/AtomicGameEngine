import * as ts from "typescript";
import * as Lint from "../lint";
export declare class Rule extends Lint.Rules.AbstractRule {
    static BRACE_FAILURE_STRING: string;
    static CATCH_FAILURE_STRING: string;
    static ELSE_FAILURE_STRING: string;
    static WHITESPACE_FAILURE_STRING: string;
    apply(sourceFile: ts.SourceFile): Lint.RuleFailure[];
}
