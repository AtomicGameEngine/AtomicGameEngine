//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

import EditorUI = require("../EditorUI");
import Preferences = require("../../editor/Preferences");

class PlayerOutput extends Atomic.UIWindow {

    output: Atomic.UIEditField;
    closeOnStop: Atomic.UICheckBox;
    errorsFileLine: Array<string>;

    constructor() {

        super();

        var view = EditorUI.getView();
        view.addChild(this);

        this.text = "Player Output";

        this.load("AtomicEditor/editor/ui/playeroutput.tb.txt");

        this.output = <Atomic.UIEditField> this.getWidget("output");
        this.closeOnStop = <Atomic.UICheckBox> this.getWidget("closeonstop");

        this.closeOnStop.value = Preferences.getInstance().editorFeatures.closePlayerLog ? 1 : 0;

        this.errorsFileLine = new Array();

        (<Atomic.UIButton>this.getWidget("closebutton")).onClick = () => {

            this.close();

        };

        this.subscribeToEvent(this, Atomic.UIWidgetEvent((data) => this.handleWidgetEvent(data)));
        this.subscribeToEvent(Editor.EditorPlayerLogEvent((ev: Editor.EditorPlayerLogEvent) => this.handlePlayerLog(ev)));

        this.resizeToFitContent();
        this.center();
        this.setFocus();

    }

    handlePlayerLog(ev: Editor.EditorPlayerLogEvent) {

        var text = this.output.text;

        if (text.length > 32768)
            this.output.text = "";


        this.output.appendText( this.wakeOnError(ev.message) + "\n");
        this.output.scrollTo(0, 0xffffff);
    }


    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CLICK) {
            var id = ev.target.id;
            if (id == "closeonstop") {
                Preferences.getInstance().editorFeatures.closePlayerLog = this.closeOnStop.value > 0 ? true : false;
                Preferences.getInstance().write();
                return true;
            } else {
                if ( id.indexOf("ERR") > -1 ) {  // ERRxxx this is an encoded error index id
                    var indx = parseInt( id.replace ("ERR", ""));  // remove the ERR string so it will parse
                    var colonpos = this.errorsFileLine[indx].indexOf( ":");  // split the filename and line
                    var fn = ToolCore.toolSystem.project.projectPath + "Resources/" + this.errorsFileLine[indx].substr(0, colonpos) + ".js";
                    var ln = this.errorsFileLine[indx].substr(colonpos + 1, (this.errorsFileLine[indx].length - colonpos));
                    var line = parseInt(ln);
                    this.sendEvent(Editor.EditorEditResourceEventData({ path: fn, lineNumber: line }));
                }
            }
        }
    }

    wakeOnError ( message: string ) {  // look for Errors in the transcript, insert links to bring up the offending file
        var ecnum = message.indexOf("ERROR");
        if ( ecnum == -1 ) return message;  // send it back if no "Error" string is present

        if ( this.errorsFileLine.length > 100 ) return message;  // limit the number of unique errors, geez

        var linenum;
        var linepos = message.indexOf("Line", ecnum);  // find the Line token to harvest the line number
        if ( linepos > -1 ) { // we found it
           var lnsrt = message.indexOf(" ", linepos + 1 );  // find the next 2 spaces, and substr it
           var lnstp = message.indexOf(" ", lnsrt + 1 );
           linenum = ":" + message.substr(lnsrt, lnstp - lnsrt).trim() + ")";  // create our next search string
        }
        else return message;  // send it back original if the Line signature is not present

        var fnpos = message.indexOf( linenum );  // find the :linenumber) position
        if ( fnpos == -1 ) return message;  // more checking
        var fnsrt = fnpos - 1;  // find the start of the string, the hard way.
        while (message.charAt(fnsrt) != "(" && message.charAt(fnsrt) != " " && message.charAt(fnsrt) != "\n" && fnsrt > 0)
            fnsrt--;
        if (message.charAt(fnsrt) == "(")  fnsrt++; // get rid of the open paren that is there now with Duk 2.x  
        var fnstp = fnpos + 1; // find the end of the string
        while (message.charAt(fnstp) != ")" && message.charAt(fnstp) != " " && message.charAt(fnstp) != "\n" && fnstp < message.length - 1 )
            fnstp++;
        var errFnLn = message.substr(fnsrt, fnstp - fnsrt).trim();  // and this is your problem, right there.

        var link = this.errorsFileLine.indexOf(errFnLn);  // see if we already have this signature 
        if ( link == -1 ) {  // if not logged yet, do so, otherwise use an existing link
            this.errorsFileLine.push(errFnLn);
            link = this.errorsFileLine.length - 1;
        }

        var lnkwidget = ` <widget TBSkinImage: skin: MagnifierBitmap, text: "..." id: ERR` + link.toString() + ` > `;
        var newMessage = message.replace(errFnLn, errFnLn + lnkwidget);

        return newMessage;
    };

}

export = PlayerOutput;
