import HostInteropType from "../interop";


interface Breakpoint {
    fileName: string;
    lineNumber: number;
}

class BreakpointDecoratorManager {
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
            this.addBreakpointDecoration(fileName, lineNumber, true);
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
/*
 *  Duktape debugger web client
 *
 *  Talks to the NodeJS server using socket.io.
 *
 *  http://unixpapa.com/js/key.html
 */
export default class DuktapeDebugger {
    // Monaco editor
    editor: monaco.editor.IStandaloneCodeEditor;
    socket: SocketIOClient.Socket;
    breakpointDecorator: BreakpointDecoratorManager;

    // Update interval for custom source highlighting.
    SOURCE_UPDATE_INTERVAL = 350;

    // Source view
    activeFileName = null;          // file that we want to be loaded in source view
    activeLine = null;              // scroll to line once file has been loaded
    activeHighlight = null;         // line that we want to highlight (if any)
    loadedFileName = null;          // currently loaded (shown) file
    loadedLineCount = 0;            // currently loaded file line count
    loadedFileExecuting = false;    // true if currFileName (loosely) matches loadedFileName
    loadedLinePending = null;       // if set, scroll loaded file to requested line
    highlightLine = null;           // highlight line
    sourceEditedLines = [];         // line numbers which have been modified
    // (added classes etc, tracked for removing)
    sourceUpdateInterval = null;    // timer for updating source view
    sourceFetchXhr = null;          // current AJAX request for fetching a source file (if any)
    forceButtonUpdate = false;      // hack to reset button states
    bytecodeDialogOpen = false;     // bytecode dialog active
    bytecodeIdxHighlight = null;    // index of currently highlighted line (or null)
    bytecodeIdxInstr = 0;           // index to first line of bytecode instructions

    // Execution state
    prevState = null;               // previous execution state ("paused", "running", etc)
    prevAttached = null;            // previous debugger attached state (true, false, null)
    currFileName = null;            // current filename being executed
    currFuncName = null;            // current function name being executed
    currLine = 0;                   // current line being executed
    currPc = 0;                     // current bytecode PC being executed
    // current execution state ("paused", "running", "detached", etc)
    currState: "connected" | "disconnected" | "running" | "paused" | "reconnecting";
    currAttached = false;           // current debugger attached state (true or false)
    currLocals = [];                // current local variables
    currCallstack = [];             // current callstack (from top to bottom)
    currBreakpoints: Breakpoint[] = [];           // current breakpoints
    startedRunning = 0;             // timestamp when last started running (if running)
    // (used to grey out the source file if running for long enough)

    /*
     *  Helpers
     */

    formatBytes(x) {
        if (x < 1024) {
            return String(x) + " bytes";
        } else if (x < 1024 * 1024) {
            return (x / 1024).toPrecision(3) + " kB";
        } else {
            return (x / (1024 * 1024)).toPrecision(3) + " MB";
        }
    }

    /*
     *  Source view periodic update handling
     */

    doSourceUpdate() {
        var elem;

        // Remove previously added custom classes
        this.sourceEditedLines.forEach((linenum) => {
            elem = $("#source-code div")[linenum - 1];
            if (elem) {
                elem.classList.remove("breakpoint");
                elem.classList.remove("execution");
                elem.classList.remove("highlight");
            }
        });
        this.sourceEditedLines.length = 0;

        // If we"re executing the file shown, highlight current line
        if (this.loadedFileExecuting) {
            this.editor.revealLineInCenterIfOutsideViewport(this.currLine);
            this.editor.setPosition(new monaco.Position(this.currLine, 0));
            this.sourceEditedLines.push(this.currLine);

            elem = $("#source-code div")[this.currLine - 1];
            if (elem) {
                //sourceEditedLines.push(currLine);
                elem.classList.add("execution");
            }
        }

        // Add breakpoints
        this.breakpointDecorator.setCurrentFileName(this.loadedFileName);
        this.currBreakpoints.forEach((bp) => {
            if (bp.fileName === this.loadedFileName) {
                this.breakpointDecorator.addBreakpointDecoration(bp.fileName, bp.lineNumber);
                this.sourceEditedLines.push(bp.lineNumber);
            }
        });

        if (this.highlightLine !== null) {
            elem = $("#source-code div")[this.highlightLine - 1];
            if (elem) {
                this.sourceEditedLines.push(this.highlightLine);
                elem.classList.add("highlight");
            }
        }

        // Bytecode dialog highlight
        if (this.loadedFileExecuting && this.bytecodeDialogOpen && this.bytecodeIdxHighlight !== this.bytecodeIdxInstr + this.currPc) {
            if (typeof this.bytecodeIdxHighlight === "number") {
                $("#bytecode-preformatted div")[this.bytecodeIdxHighlight].classList.remove("highlight");
            }
            this.bytecodeIdxHighlight = this.bytecodeIdxInstr + this.currPc;
            $("#bytecode-preformatted div")[this.bytecodeIdxHighlight].classList.add("highlight");
        }

        // If no-one requested us to scroll to a specific line, finish.
        if (this.loadedLinePending == null) {
            return;
        }

        // Scroll to the requested line
        var reqLine = this.loadedLinePending;
        this.loadedLinePending = null;

        this.editor.revealLineInCenterIfOutsideViewport(reqLine);
        this.editor.setPosition(new monaco.Position(reqLine, 0));
    }

    /*
     *  UI update handling when exec-status update arrives
     */

    doUiUpdate() {
        var now = Date.now();

        // Note: loadedFileName can be either from target or from server, but they
        // must match exactly.  We could do a loose match here, but exact matches
        // are needed for proper breakpoint handling anyway.
        this.loadedFileExecuting = (this.loadedFileName === this.currFileName);

        // If we just started running, store a timestamp so we can grey out the
        // source view only if we execute long enough (i.e. we"re not just
        // stepping).
        if (this.currState !== this.prevState && this.currState === "running") {
            this.startedRunning = now;
        }

        // If we just became paused, check for eval watch
        if (this.currState !== this.prevState && this.currState === "paused") {
            if ($("#eval-watch").is(":checked")) {
                this.submitEval();  // don"t clear eval input
            }
        }

        // Update current execution state
        if (this.currFileName === "" && this.currLine === 0) {
            $("#current-fileline").text("");
        } else {
            $("#current-fileline").text(String(this.currFileName) + ":" + String(this.currLine));
        }
        if (this.currFuncName === "" && this.currPc === 0) {
            $("#current-funcpc").text("");
        } else {
            $("#current-funcpc").text(String(this.currFuncName) + "() pc " + String(this.currPc));
        }
        $("#current-state").text(String(this.currState));

        // Update buttons
        if (this.currState !== this.prevState || this.currAttached !== this.prevAttached || this.forceButtonUpdate) {
            $("#stepinto-button").prop("disabled", !this.currAttached || this.currState !== "paused");
            $("#stepover-button").prop("disabled", !this.currAttached || this.currState !== "paused");
            $("#stepout-button").prop("disabled", !this.currAttached || this.currState !== "paused");
            $("#resume-button").prop("disabled", !this.currAttached || this.currState !== "paused");
            $("#pause-button").prop("disabled", !this.currAttached || this.currState !== "running");
            $("#attach-button").prop("disabled", this.currAttached);
            if (this.currAttached) {
                $("#attach-button").removeClass("enabled");
            } else {
                $("#attach-button").addClass("enabled");
            }
            $("#detach-button").prop("disabled", !this.currAttached);
            $("#eval-button").prop("disabled", !this.currAttached);
            $("#add-breakpoint-button").prop("disabled", !this.currAttached);
            $("#delete-all-breakpoints-button").prop("disabled", !this.currAttached);
            $(".delete-breakpoint-button").prop("disabled", !this.currAttached);
            $("#putvar-button").prop("disabled", !this.currAttached);
            $("#getvar-button").prop("disabled", !this.currAttached);
            $("#heap-dump-download-button").prop("disabled", !this.currAttached);
        }
        if (this.currState !== "running" || this.forceButtonUpdate) {
            // Remove pending highlight once we"re no longer running.
            $("#pause-button").removeClass("pending");
            $("#eval-button").removeClass("pending");
        }
        this.forceButtonUpdate = false;

        // Make source window grey when running for a longer time, use a small
        // delay to avoid flashing grey when stepping.
        if (this.currState === "running" && now - this.startedRunning >= 500) {
            $("#source-pre").removeClass("notrunning");
            $("#current-state").removeClass("notrunning");
        } else {
            $("#source-pre").addClass("notrunning");
            $("#current-state").addClass("notrunning");
        }

        // Force source view to match currFileName only when running or when
        // just became paused (from running or detached).
        var fetchSource = false;
        if (typeof this.currFileName === "string") {
            if (this.currState === "running" ||
                (this.prevState !== "paused" && this.currState === "paused") ||
                (this.currAttached !== this.prevAttached)) {
                if (this.activeFileName !== this.currFileName) {
                    fetchSource = true;
                    this.activeFileName = this.currFileName;
                    this.activeLine = this.currLine;
                    this.activeHighlight = null;
                    this.requestSourceRefetch();
                }
            }
        }

        // Force line update (scrollTop) only when running or just became paused.
        // Otherwise let user browse and scroll source files freely.
        if (!fetchSource) {
            if ((this.prevState !== "paused" && this.currState === "paused") ||
                this.currState === "running") {
                this.loadedLinePending = this.currLine || 0;
            }
        }
    }

    deleteAllBreakpoints() {
        this.socket.emit("delete-all-breakpoints");
        this.breakpointDecorator.clearBreakpointDecorations();
    }

    addBreakpoint(fileName: string, lineNumber: number) {
        this.breakpointDecorator.addBreakpointDecoration(fileName, lineNumber);
        this.socket.emit("add-breakpoint", {
            fileName,
            lineNumber
        });
    }

    toggleBreakpoint(fileName: string, lineNumber: number) {
        this.breakpointDecorator.toggleBreakpoint(fileName, lineNumber);
        this.socket.emit("toggle-breakpoint", {
            fileName,
            lineNumber
        });
    }

    deleteBreakpoint(fileName: string, lineNumber: number) {
        this.breakpointDecorator.removeBreakpointDecoration(fileName, lineNumber);
        this.socket.emit("delete-breakpoint", {
            fileName,
            lineNumber
        });
    }

    initSocket() {
        /*
         *  Init socket.io and add handlers
         */

        this.socket = io.connect("http://localhost:9092");  // returns a Manager

        setInterval(() => {
            this.socket.emit("keepalive", {
                userAgent: (navigator || {} as Navigator).userAgent
            });
        }, 30000);

        this.socket.on("connect", () => {
            $("#socketio-info").text("connected");
            this.currState = "connected";

            this.fetchSourceList();
        });
        this.socket.on("disconnect", () => {
            $("#socketio-info").text("not connected");
            this.currState = "disconnected";
        });
        this.socket.on("reconnecting", () => {
            $("#socketio-info").text("reconnecting");
            this.currState = "reconnecting";
        });
        this.socket.on("error", (err) => {
            $("#socketio-info").text(err);
        });

        this.socket.on("replaced", () => {
            // XXX: how to minimize the chance we"ll further communciate with the
            // server or reconnect to it?  socket.reconnection()?

            // We"d like to window.close() here but can"t (not allowed from scripts).
            // Alert is the next best thing.
            alert("Debugger connection replaced by a new one, do you have multiple tabs open? If so, please close this tab.");
        });

        this.socket.on("keepalive", (msg) => {
            // Not really interesting in the UI
            // $("#server-info").text(new Date() + ": " + JSON.stringify(msg));
        });

        this.socket.on("basic-info", (msg) => {
            $("#duk-version").text(String(msg.duk_version));
            $("#duk-git-describe").text(String(msg.duk_git_describe));
            $("#target-info").text(String(msg.target_info));
            $("#endianness").text(String(msg.endianness));
        });

        this.socket.on("exec-status", (msg) => {
            // Not 100% reliable if callstack has several functions of the same name
            if (this.bytecodeDialogOpen && (this.currFileName != msg.fileName || this.currFuncName != msg.funcName)) {
                this.socket.emit("get-bytecode", {});
            }

            this.currFileName = msg.fileName;
            this.currFuncName = msg.funcName;
            this.currLine = msg.line;
            this.currPc = msg.pc;
            this.currState = msg.state;
            this.currAttached = msg.attached;

            // Duktape now restricts execution status updates quite effectively so
            // there"s no need to rate limit UI updates now.

            this.doUiUpdate();

            this.prevState = this.currState;
            this.prevAttached = this.currAttached;
        });

        // Update the "console" output based on lines sent by the server.  The server
        // rate limits these updates to keep the browser load under control.  Even
        // better would be for the client to pull this (and other stuff) on its own.
        this.socket.on("output-lines", (msg) => {
            var elem = $("#output");
            var i, n, ent;

            elem.empty();
            for (i = 0, n = msg.length; i < n; i++) {
                ent = msg[i];
                if (ent.type === "print") {
                    elem.append($("<div></div>").text(ent.message));
                } else if (ent.type === "alert") {
                    elem.append($("<div class='alert'></div>").text(ent.message));
                } else if (ent.type === "log") {
                    elem.append($("<div class='log loglevel' + ent.level + ''></div>").text(ent.message));
                } else if (ent.type === "debugger-info") {
                    elem.append($("<div class='debugger-info'><div>").text(ent.message));
                } else if (ent.type === "debugger-debug") {
                    elem.append($("<div class='debugger-debug'><div>").text(ent.message));
                } else {
                    elem.append($("<div></div>").text(ent.message));
                }
            }

            // http://stackoverflow.com/questions/14918787/jquery-scroll-to-bottom-of-div-even-after-it-updates
            // Stop queued animations so that we always scroll quickly to bottom
            $("#output").stop(true);
            $("#output").animate({ scrollTop: $("#output")[0].scrollHeight }, 1000);
        });

        this.socket.on("callstack", (msg) => {
            var elem = $("#callstack");
            var s1, s2, div;

            this.currCallstack = msg.callstack;

            elem.empty();
            msg.callstack.forEach((e) => {
                s1 = $("<a class='rest'></a>").text(e.fileName + ":" + e.lineNumber + " (pc " + e.pc + ")");  // float
                s1.on("click", () => {
                    this.activeFileName = e.fileName;
                    this.activeLine = e.lineNumber || 1;
                    this.activeHighlight = this.activeLine;
                    this.requestSourceRefetch();
                });
                s2 = $("<span class='func'></span>").text(e.funcName + "()");
                div = $("<div></div>");
                div.append(s1);
                div.append(s2);
                elem.append(div);
            });
        });

        this.socket.on("locals", (msg) => {
            var elem = $("#locals");
            var s1, s2, div;
            var i, n, e;

            this.currLocals = msg.locals;

            elem.empty();
            for (i = 0, n = msg.locals.length; i < n; i++) {
                e = msg.locals[i];
                s1 = $("<span class='value'></span>").text(e.value);  // float
                s2 = $("<span class='key'></span>").text(e.key);
                div = $("<div></div>");
                div.append(s1);
                div.append(s2);
                elem.append(div);
            }
        });

        this.socket.on("debug-stats", (msg) => {
            $("#debug-rx-bytes").text(this.formatBytes(msg.rxBytes));
            $("#debug-rx-dvalues").text(msg.rxDvalues);
            $("#debug-rx-messages").text(msg.rxMessages);
            $("#debug-rx-kbrate").text((msg.rxBytesPerSec / 1024).toFixed(2));
            $("#debug-tx-bytes").text(this.formatBytes(msg.txBytes));
            $("#debug-tx-dvalues").text(msg.txDvalues);
            $("#debug-tx-messages").text(msg.txMessages);
            $("#debug-tx-kbrate").text((msg.txBytesPerSec / 1024).toFixed(2));
        });


        this.socket.on("breakpoints", (msg) => {
            var elem = $("#breakpoints");
            var div;
            var sub;

            this.currBreakpoints = msg.breakpoints;

            elem.empty();

            // First line is special
            div = $("<div></div>");
            sub = $("<button id='delete-all-breakpoints-button'></button>").text("Delete all breakpoints");
            sub.on("click", () => {
                this.deleteAllBreakpoints();
            });
            div.append(sub);
            //sub = $("<input id='add-breakpoint-file'></input>").val("file.js");
            //div.append(sub);
            //sub = $("<span></span>").text(":");
            //div.append(sub);
            //sub = $("<input id='add-breakpoint-line'></input>").val("123");
            //div.append(sub);
            //sub = $("<button id='add-breakpoint-button'></button>").text("Add breakpoint");
            //sub.on("click", () => {
                //this.addBreakpoint($("#add-breakpoint-file").val(), Number($("#add-breakpoint-line").val()));
            //});
            //div.append(sub);
            //sub = $("<span id='breakpoint-hint'></span>").text("or dblclick source");
            //div.append(sub);
            elem.append(div);

            // Active breakpoints follow
            msg.breakpoints.forEach((bp) => {
                var div;
                var sub;

                div = $("<div class='breakpoint-line'></div>");
                sub = $("<button class='delete-breakpoint-button'></button>").text("Delete");
                sub.on("click", () => {
                    this.deleteBreakpoint(bp.fileName, bp.lineNumber);
                });
                div.append(sub);
                sub = $("<a></a>").text((bp.fileName || "?") + ":" + (bp.lineNumber || 0));
                sub.on("click", () => {
                    this.activeFileName = bp.fileName || "";
                    this.activeLine = bp.lineNumber || 1;
                    this.activeHighlight = this.activeLine;
                    this.requestSourceRefetch();
                });
                div.append(sub);
                elem.append(div);
            });

            this.forceButtonUpdate = true;
            this.doUiUpdate();
        });

        this.socket.on("eval-result", (msg) => {
            $("#eval-output").text((msg.error ? "ERROR: " : "") + msg.result);

            // Remove eval button "pulsating" glow when we get a result
            $("#eval-button").removeClass("pending");
        });

        this.socket.on("getvar-result", (msg) => {
            $("#var-output").text(msg.found ? msg.result : "NOTFOUND");
        });

        this.socket.on("bytecode", (msg) => {
            var elem, div;

            elem = $("#bytecode-preformatted");
            elem.empty();

            msg.preformatted.split("\n").forEach((line, idx) => {
                div = $("<div></div>");
                div.text(line);
                elem.append(div);
            });

            this.bytecodeIdxHighlight = null;
            this.bytecodeIdxInstr = msg.idxPreformattedInstructions;
        });

        $("#stepinto-button").click(() => {
            this.socket.emit("stepinto", {});
        });

        $("#stepover-button").click(() => {
            this.socket.emit("stepover", {});
        });

        $("#stepout-button").click(() => {
            this.socket.emit("stepout", {});
        });

        $("#pause-button").click(() => {
            this.socket.emit("pause", {});

            // Pause may take seconds to complete so indicate it is pending.
            $("#pause-button").addClass("pending");
        });

        $("#resume-button").click(() => {
            this.socket.emit("resume", {});
        });

        $("#attach-button").click(() => {
            this.socket.emit("attach", {});
        });

        $("#detach-button").click(() => {
            this.socket.emit("detach", {});
        });

        $("#about-button").click(() => {
            $("#about-dialog").dialog("open");
        });

        $("#show-bytecode-button").click(() => {
            this.bytecodeDialogOpen = true;
            $("#bytecode-dialog").dialog("open");

            let elem = $("#bytecode-preformatted");
            elem.empty().text("Loading bytecode...");

            this.socket.emit("get-bytecode", {});
        });
        $("#eval-button").click(() => {
            this.submitEval();
            $("#eval-input").val("");
        });

        $("#getvar-button").click(() => {
            this.socket.emit("getvar", { varname: $("#varname-input").val() });
        });

        $("#putvar-button").click(() => {
            // The variable value is parsed as JSON right now, but it"d be better to
            // also be able to parse buffer values etc.
            var val = JSON.parse($("#varvalue-input").val());
            this.socket.emit("putvar", { varname: $("#varname-input").val(), varvalue: val });
        });

        $("#source-code").dblclick((event) => {
            var target = event.target;
            var elems = $("#source-code div");
            var i, n;
            var line = 0;

            // XXX: any faster way; elems doesn"t have e.g. indexOf()
            for (i = 0, n = elems.length; i < n; i++) {
                if (target === elems[i]) {
                    line = i + 1;
                }
            }

            this.toggleBreakpoint(this.loadedFileName, line);
        });
    }

    submitEval() {
        this.socket.emit("eval", { input: $("#eval-input").val() });

        // Eval may take seconds to complete so indicate it is pending.
        $("#eval-button").addClass("pending");
    }


    setSourceText(data) {
        /* TSH
        var elem, div;

        elem = $("#source-code");
        elem.empty();
        data.split("\n").forEach(function (line) {
            div = $("<div></div>");
            div.text(line);
            elem.append(div);
        });
        */

        this.editor.deltaDecorations([], []);
        this.editor.getModel().setValue(data);
        this.sourceEditedLines = [];
    }

    /*
     *  AJAX request handling to fetch source files
     */
    requestSourceFile(fileName: string, lineNumber: number) {
        // If previous update is pending, abort and start a new one.
        if (this.sourceFetchXhr) {
            this.sourceFetchXhr.abort();
            this.sourceFetchXhr = null;
        }

        console.log(`Retrieving File: ${fileName}`);
        // get the code
        return HostInteropType.getInstance().getFileResource("Resources/" + fileName).then((data: string) => {
            this.loadedFileName = fileName;
            this.loadedLineCount = data.split("\n").length;  // XXX: ignore issue with last empty line for now
            this.loadedFileExecuting = (this.loadedFileName === this.currFileName);
            this.setSourceText(data);
            this.loadedLinePending = this.activeLine || 1;
            this.highlightLine = this.activeHighlight;  // may be null
            this.activeLine = null;
            this.activeHighlight = null;
            this.doSourceUpdate();

        }).catch((e: Editor.ClientExtensions.AtomicErrorMessage) => {
            console.log("Error loading code: " + e.error_message);
            // Not worth alerting about because source fetch errors happen
            // all the time, e.g. for dynamically evaluated code.

            this.sourceFetchXhr = null;

            // XXX: prevent retry of no-such-file by negative caching?
            this.loadedFileName = fileName;
            this.loadedLineCount = 1;
            this.loadedFileExecuting = false;
            this.setSourceText("// Cannot load source file: " + fileName);
            this.loadedLinePending = 1;
            this.activeLine = null;
            this.activeHighlight = null;
            this.doSourceUpdate();
        });
    }

    requestSourceRefetch() {
        if (!this.activeFileName) {
            return;
        }

        this.requestSourceFile(this.activeFileName, this.activeLine).then(() => {
            // XXX: hacky transition, make source change visible
            $("#source-pre").fadeTo("fast", 0.25, () => {
                $("#source-pre").fadeTo("fast", 1.0);
            });
        }).catch((e: Editor.ClientExtensions.AtomicErrorMessage) => {
            // XXX: error transition here
            console.log("Error loading code: " + e.error_message);
            $("#source-pre").fadeTo("fast", 0.25, function() {
                $("#source-pre").fadeTo("fast", 1.0);
            });
        });
    }

    /*
     *  AJAX request for fetching the source list
     */

    fetchSourceList() {
        /* TODO: Fix Ajax
        $.ajax({
            type: "POST",
            url: "/sourceList",
            data: JSON.stringify({}),
            contentType: "application/json",
            success: function(data, status, jqxhr) {
                var elem = $("#source-select");

                data = JSON.parse(data);

                elem.empty();
                var opt = $("<option></option>").attr({ "value": "__none__" }).text("No source file selected");
                elem.append(opt);
                data.forEach(function(ent) {
                    var opt = $("<option></option>").attr({ "value": ent }).text(ent);
                    elem.append(opt);
                });
                elem.change(function() {
                    activeFileName = elem.val();
                    activeLine = 1;
                    requestSourceRefetch();
                });
            },
            error: function(jqxhr, status, err) {
                // This is worth alerting about as the UI is somewhat unusable
                // if we don"t get a source list.

                alert("Failed to load source list: " + err);
            },
            dataType: "text"
        });
        */
    }

    /*
     *  Initialization
     */
    initialize(editorProvided) {
        this.editor = editorProvided;
        this.breakpointDecorator = new BreakpointDecoratorManager(editorProvided);
        var showAbout = true;

        this.initSocket();

        // Source is updated periodically.  Other code can also call doSourceUpdate()
        // directly if an immediate update is needed.
        this.sourceUpdateInterval = setInterval(this.doSourceUpdate.bind(this), this.SOURCE_UPDATE_INTERVAL);
        this.editor.onMouseMove((e) => {
            var targetZone = e.target.toString();
            if (targetZone.indexOf("GUTTER_GLYPH_MARGIN") != -1) {
                var line = e.target.position.lineNumber;
                this.breakpointDecorator.updateMarginHover(line);
            } else {
                this.breakpointDecorator.removeMarginHover();
            }
        });

        this.editor.onMouseDown((e) => {
            var targetZone = e.target.toString();
            if (targetZone.indexOf("GUTTER_GLYPH_MARGIN") != -1) {
                var line = e.target.position.lineNumber;
                this.toggleBreakpoint(this.loadedFileName, line);
            }

        });

        // About dialog, shown automatically on first startup.
        $("#about-dialog").dialog({
            autoOpen: false,
            hide: "fade",  // puff
            show: "fade",  // slide, puff
            width: 500,
            height: 300
        });

        // Bytecode dialog
        $("#bytecode-dialog").dialog({
            autoOpen: false,
            hide: "fade",  // puff
            show: "fade",  // slide, puff
            width: 700,
            height: 600,
            close: () => {
                this.bytecodeDialogOpen = false;
                this.bytecodeIdxHighlight = null;
                this.bytecodeIdxInstr = 0;
            }
        });

        // http://diveintohtml5.info/storage.html
        if (typeof localStorage !== "undefined") {
            if (localStorage.getItem("about-shown")) {
                showAbout = false;
            } else {
                localStorage.setItem("about-shown", "yes");
            }
        }
        if (showAbout) {
            $("#about-dialog").dialog("open");
        }

        // onclick handler for exec status text
        function loadCurrFunc() {
            this.activeFileName = this.currFileName;
            this.activeLine = this.currLine;
            this.requestSourceRefetch();
        }
        $("#exec-other").on("click", loadCurrFunc);

        // Enter handling for eval input
        // https://forum.jquery.com/topic/bind-html-input-to-enter-key-keypress
        $("#eval-input").keypress((event) => {
            if (event.keyCode == 13) {
                this.submitEval();
                $("#eval-input").val("");
            }
        });

        // Eval watch handling
        $("#eval-watch").change(() => {
            // nop
        });

        this.forceButtonUpdate = true;
        this.doUiUpdate();
    };

}
