
import ModalWindow = require("./ModalWindow");

class NewProject extends ModalWindow {

    constructor() {

      super();

      this.text = "Project Type";

      this.load("AtomicEditor/editor/ui/newproject.tb.txt");

      this.resizeToFitContent();
      this.center();

    }

}


export = NewProject;
