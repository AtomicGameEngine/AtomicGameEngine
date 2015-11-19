
import SerializableEditType = require("./SerializableEditType");

class SelectionSectionUI extends Atomic.UILayout {

    editType: SerializableEditType;

    createUI(editType: SerializableEditType) {

      this.editType = editType;

    }

}

export = SelectionSectionUI;
