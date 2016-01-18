import { findConfiguration as config } from "./configuration";
import { ILinterOptions, LintResult } from "./lint";
declare class Linter {
    static VERSION: string;
    static findConfiguration: typeof config;
    private fileName;
    private source;
    private options;
    constructor(fileName: string, source: string, options: ILinterOptions);
    lint(): LintResult;
    private containsRule(rules, rule);
}
declare namespace Linter {
}
export = Linter;
