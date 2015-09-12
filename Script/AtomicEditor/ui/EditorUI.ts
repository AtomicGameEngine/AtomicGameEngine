//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorEvents = require("editor/EditorEvents");
import MainFrame = require("./frames/MainFrame");
import ModalOps = require("./modal/ModalOps");
import Shortcuts = require("./Shortcuts");

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

    this.subscribeToEvent(EditorEvents.ModalError, (event:EditorEvents.ModalErrorEvent) => {
      this.showModalError(event.title, event.message);
    })

  }

  showModalError(windowText:string, message:string)
  {
      var window = new Atomic.UIMessageWindow(this.view, "modal_error");
      window.show(windowText, message, Atomic.UI_MESSAGEWINDOW_SETTINGS_OK, true, 640, 360);
  }

  view: Atomic.UIView;
  mainframe: MainFrame;
  modalOps: ModalOps;
  shortcuts: Shortcuts;

}
