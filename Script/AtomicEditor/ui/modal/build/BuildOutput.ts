
import EditorUI = require("../../EditorUI");
import ModalWindow = require("../ModalWindow");

class BuildOutput extends ModalWindow {

    constructor(buildBase:ToolCore.BuildBase) {

        super();

        this.buildBase = buildBase;

        this.init("Program Output", "AtomicEditor/editor/ui/programoutput.tb.txt");

        this.outputField = <Atomic.UIEditField> this.getWidget("output");

        this.resizeToFitContent();
        this.center();

        this.subscribeToEvent(buildBase, "BuildOutput", (ev:ToolCore.BuildOutputEvent) => {

          console.log(ev.text);
          this.text += ev.text;
          this.outputField.text = this.text;
          this.outputField.scrollTo(0, 0xffffff);

        });

    }

    text:string;
    buildBase:ToolCore.BuildBase;
    outputField:Atomic.UIEditField;

}

export = BuildOutput;
