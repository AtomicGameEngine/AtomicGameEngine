
import EditorEvents = require("../../editor/EditorEvents");
import EditorUI = require("../EditorUI");

class PlayerOutput extends Atomic.UIWindow {

    output: Atomic.UIEditField;

    constructor() {

        super();

        var view = EditorUI.getView();
        view.addChild(this);

        this.text = "Player Output";

        this.load("AtomicEditor/editor/ui/playeroutput.tb.txt")

        this.output = <Atomic.UIEditField> this.getWidget("output");

        (<Atomic.UIButton>this.getWidget("closebutton")).onClick = () => {

            this.close();

        }

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));
        this.subscribeToEvent(EditorEvents.PlayerLog, (ev: EditorEvents.PlayerLogEvent) => this.handlePlayerLog(ev));

        this.resizeToFitContent();
        this.center();
        this.setFocus();

    }

    handlePlayerLog(ev: EditorEvents.PlayerLogEvent) {

        var text = this.output.text;
        if (text.length > 32768)
            text = "";
        text += ev.message;
        this.output.text = text + "\n";
        this.output.scrollTo(0, 0xffffff);
    }


    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

    }

}

export = PlayerOutput;
