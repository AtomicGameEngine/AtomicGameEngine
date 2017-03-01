export interface Breakpoint {
    fileName: string;
    lineNumber: number;
}

export default class BreakpointDecoratorManager {
    currEditorDecorations: monaco.editor.IModelDeltaDecoration[] = [];
    currEditorDecorationIds: string[] = [];
    currFileName: string;

    constructor(private editor: monaco.editor.IStandaloneCodeEditor) { }

    setCurrentFileName(fileName: string) {
        if (this.currFileName != fileName) {
            this.clearBreakpointDecorations();
        }
        this.currFileName = fileName;
    }

    clearBreakpointDecorations() {
        this.currEditorDecorations.length = 0;
        this.currEditorDecorationIds = this.editor.deltaDecorations(this.currEditorDecorationIds, this.currEditorDecorations);
    }

    removeBreakpointDecoration(fileName: string, lineNumber: number) {
        if (fileName != this.currFileName) {
            return;
        }

        const idx = this.currEditorDecorations.findIndex(d => d.range.startLineNumber == lineNumber);
        if (idx != -1) {
            this.currEditorDecorations.splice(idx);
            this.currEditorDecorationIds = this.editor.deltaDecorations(this.currEditorDecorationIds, this.currEditorDecorations);
        }
    }

    addBreakpointDecoration(fileName: string, lineNumber: number, pendingBreakpoint?: boolean) {
        if (fileName != this.currFileName) {
            return;
        }

        const requestedBreakpointClass = pendingBreakpoint ? "code-breakpoint-pending" : "code-breakpoint";
        let idx = this.currEditorDecorations.findIndex(d => d.range.startLineNumber == lineNumber);
        if (idx != -1 && this.currEditorDecorations[idx].options.glyphMarginClassName != requestedBreakpointClass) {
            this.removeBreakpointDecoration(fileName, lineNumber);
            idx = -1;
        }

        if (idx == -1) {
            this.currEditorDecorations.push({
                range: new monaco.Range(lineNumber, 1, lineNumber, 1),
                options: {
                    glyphMarginClassName: pendingBreakpoint ? "code-breakpoint-pending" : "code-breakpoint"
                }
            });

            this.currEditorDecorationIds = this.editor.deltaDecorations(this.currEditorDecorationIds, this.currEditorDecorations);
        }
    }

    getBreakpointDecorator(fileName: string, lineNumber: number) {
        return this.currEditorDecorations.find(d => d.range.startLineNumber == lineNumber && d.options.glyphMarginClassName != "code-breakpoint-margin-hover");
    }

    toggleBreakpoint(fileName: string, lineNumber: number) {
        if (fileName != this.currFileName) {
            return;
        }

        let decoration = this.getBreakpointDecorator(fileName, lineNumber);
        if (decoration) {
            this.removeBreakpointDecoration(fileName, lineNumber);
        } else {
            this.addBreakpointDecoration(fileName, lineNumber, false);
        }
    }

    updateMarginHover(lineNumber: number) {
        let idx = this.currEditorDecorations.findIndex(d => d.options.glyphMarginClassName == "code-breakpoint-margin-hover");

        if (idx != -1 && this.currEditorDecorations[idx].range.startLineNumber == lineNumber) {
            return; // nothing to do
        }

        const hover = {
            range: new monaco.Range(lineNumber, 1, lineNumber, 1),
            options: {
                glyphMarginClassName: "code-breakpoint-margin-hover"
            }
        };

        if (idx == -1) {
            this.currEditorDecorations.push(hover);
        } else {
            this.currEditorDecorations[idx] = hover;
        }

        this.currEditorDecorationIds = this.editor.deltaDecorations(this.currEditorDecorationIds, this.currEditorDecorations);
    }

    removeMarginHover() {
        let idx = this.currEditorDecorations.findIndex(d => d.options.glyphMarginClassName == "code-breakpoint-margin-hover");
        if (idx != -1) {
            this.currEditorDecorations.splice(idx);
            this.currEditorDecorationIds = this.editor.deltaDecorations(this.currEditorDecorationIds, this.currEditorDecorations);
        }
    }
}
