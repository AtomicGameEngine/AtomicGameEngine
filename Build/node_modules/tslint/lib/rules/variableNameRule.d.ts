import * as ts from "typescript";
import * as Lint from "../lint";
export declare class Rule extends Lint.Rules.AbstractRule {
    static FORMAT_FAILURE: string;
    static KEYWORD_FAILURE: string;
    apply(sourceFile: ts.SourceFile): Lint.RuleFailure[];
}
