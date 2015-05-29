
import MainFrame = require("../ui/MainFrame");
import UIEvents = require("../ui/UIEvents");


export function getEditor():Editor {
  return TheEditor;
}

var TheEditor:Editor;

export class Editor extends Atomic.JSScriptObject
{

  project: ToolCore.Project;
  view: Atomic.UIView;
  mainframe: MainFrame.MainFrame;

  loadProject(projectPath:string) {

    var system = ToolCore.getToolSystem();

    if (system.project) {

      this.sendEvent(UIEvents.MessageModalEvent,
        { type:"error", title:"Project already loaded", message:"Project already loaded"} );

    }

    system.loadProject(projectPath);

  }

  constructor() {

    super();

    TheEditor = this;

    var graphics = Atomic.getGraphics();

    this.view = new Atomic.UIView();

    this.mainframe = new MainFrame.MainFrame();

    this.view.addChild(this.mainframe);

    // set initial size
    this.mainframe.setSize(graphics.width, graphics.height);

  }

}
