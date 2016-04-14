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

import EditorEvents = require("../../editor/EditorEvents");
import EditorUI = require("../EditorUI");
import PlayerOutput = require("./PlayerOutput");

class PlayMode extends Atomic.ScriptObject {

    inErrorState: boolean;

    constructor() {

        super();

        this.subscribeToEvent("IPCJSError", (ev: Atomic.IPCJSErrorEvent) => this.handleIPCJSError(ev));
        this.subscribeToEvent(EditorEvents.PlayerStarted, (ev) => this.handlePlayerStarted(ev));

        this.subscribeToEvent("EditorPlayerRenderTextureUpdated", (ev) => this.handlePlayerRenderTextureUpdated(ev));

    }

    handlePlayerRenderTextureUpdated(ev) {

      // Create a UIWindow
      var window = new Atomic.UIWindow();
      window.settings = Atomic.UI_WINDOW_SETTINGS_TITLEBAR;
      window.text = "Scene Preview";
      window.setSize(512 + 16, 550);

      var widget = new Atomic.UITextureWidget();
      widget.texture = Atomic.editorMode.getPlayerRenderTexture();

      var tlp = new Atomic.UILayoutParams();
      tlp.width = 512;
      tlp.height = 512;
      widget.layoutParams = tlp;

      window.addChild(widget);

      EditorUI.getView().addChild(window);
      window.center();

    }

    handlePlayerStarted(ev) {

        this.inErrorState = false;

        new PlayerOutput();
    }

    handleIPCJSError(ev: Atomic.IPCJSErrorEvent) {

        if (this.inErrorState)
            return;

        this.inErrorState = true;

        var errorMessage = ev.errorFileName + " - " + ev.errorLineNumber + " : " + ev.errorMessage;
        this.sendEvent(EditorEvents.ModalError, { title: "Player JavaScript Error", message: errorMessage });

        Atomic.graphics.raiseWindow();

    }

}

export = PlayMode;
