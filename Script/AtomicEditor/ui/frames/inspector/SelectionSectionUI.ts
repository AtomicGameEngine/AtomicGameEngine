
import SerializableEditType = require("./SerializableEditType");

class SelectionSectionUI extends Atomic.UILayout {

    editType: SerializableEditType;

    refresh() {
      
    }

    createUI(editType: SerializableEditType) {

      this.editType = editType;

    }

}

export = SelectionSectionUI;
