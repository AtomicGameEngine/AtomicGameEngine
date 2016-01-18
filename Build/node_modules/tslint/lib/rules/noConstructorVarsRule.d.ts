import * as ts from "typescript";
import * as Lint from "../lint";
export declare class Rule extends Lint.Rules.AbstractRule {
    static FAILURE_STRING_PART: string;
    apply(sourceFile: ts.SourceFile): Lint.RuleFailure[];
}
export declare class NoConstructorVarsWalker extends Lint.RuleWalker {
    visitConstructorDeclaration(node: ts.ConstructorDeclaration): void;
}
