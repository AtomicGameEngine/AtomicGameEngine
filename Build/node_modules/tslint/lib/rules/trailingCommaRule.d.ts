import * as ts from "typescript";
import * as Lint from "../lint";
export declare class Rule extends Lint.Rules.AbstractRule {
    static FAILURE_STRING_NEVER: string;
    static FAILURE_STRING_ALWAYS: string;
    apply(sourceFile: ts.SourceFile): Lint.RuleFailure[];
}
