//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

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
