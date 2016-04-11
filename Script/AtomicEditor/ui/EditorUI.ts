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

import EditorEvents = require("editor/EditorEvents");
import MainFrame = require("./frames/MainFrame");
import ModalOps = require("./modal/ModalOps");
import Shortcuts = require("./Shortcuts");
import ServiceLocator from "../hostExtensions/ServiceLocator";

// this is designed with public get functions to solve
// circular dependency issues in TS

var editorUI:EditorUI;

export function getMainFrame():MainFrame {
  return editorUI.mainframe;
}

export function getModelOps():ModalOps {
  return editorUI.modalOps;
}

export function getView():Atomic.UIView {
  return editorUI.view;
}

export function getShortcuts():Shortcuts {
  return editorUI.shortcuts;
}

export function initialize() {
  editorUI = new EditorUI();
}

export function shutdown() {

  editorUI.mainframe.shutdown();
  var view = editorUI.view;
  view.deleteAllChildren();
  view.parent.removeChild(view);

}

export function showModalError(windowText:string, message:string) {
  editorUI.showModalError(windowText, message);
}

export function getCurrentResourceEditor():Editor.ResourceEditor {
    return getMainFrame().resourceframe.currentResourceEditor;
}


class EditorUI extends Atomic.ScriptObject {

  constructor() {

    super();

    var graphics = Atomic.getGraphics();

    this.view = new Atomic.UIView();

    this.mainframe = new MainFrame();

    this.view.addChild(this.mainframe);

    this.subscribeToEvent("ScreenMode", (ev:Atomic.ScreenModeEvent) => {

      this.mainframe.setSize(ev.width, ev.height);

    });

    // set initial size
    this.mainframe.setSize(graphics.width, graphics.height);

    this.modalOps = new ModalOps();
    this.shortcuts = new Shortcuts();

    // Hook the service locator into the event system and give it the ui objects it needs
    ServiceLocator.uiServices.init(this.mainframe.menu, this.modalOps);
    ServiceLocator.subscribeToEvents(this.mainframe);

    this.subscribeToEvent(EditorEvents.ModalError, (event:EditorEvents.ModalErrorEvent) => {
      this.showModalError(event.title, event.message);
    });

  }

  showModalError(windowText:string, message:string) {
      var window = new Atomic.UIMessageWindow(this.view, "modal_error");
      window.show(windowText, message, Atomic.UI_MESSAGEWINDOW_SETTINGS_OK, true, 640, 360);
  }

  view: Atomic.UIView;
  mainframe: MainFrame;
  modalOps: ModalOps;
  shortcuts: Shortcuts;

}
