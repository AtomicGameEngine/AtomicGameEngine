import * as configuration from "./configuration";
import * as formatters from "./formatters";
import * as linter from "./tslint";
import * as rules from "./rules";
import { RuleFailure } from "./language/rule/rule";
export * from "./language/rule/rule";
export * from "./enableDisableRules";
export * from "./formatterLoader";
export * from "./ruleLoader";
export * from "./language/utils";
export * from "./language/languageServiceHost";
export * from "./language/walker";
export * from "./language/formatter/formatter";
export declare var Configuration: typeof configuration;
export declare var Formatters: typeof formatters;
export declare var Linter: typeof linter;
export declare var Rules: typeof rules;
export interface LintResult {
    failureCount: number;
    failures: RuleFailure[];
    format: string;
    output: string;
}
export interface ILinterOptions {
    configuration: any;
    formatter: string;
    formattersDirectory: string;
    rulesDirectory: string | string[];
}
