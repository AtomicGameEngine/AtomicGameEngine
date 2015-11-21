//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import SerializableEditType = require("./SerializableEditType");
import SelectionInspector = require("./SelectionInspector");

class JSComponentEditType extends SerializableEditType {

    compareTypes(otherType: SerializableEditType, multiSelect: boolean = false): boolean {

        if (this.typeName != otherType.typeName) {
            return false;
        }

        if (!multiSelect)
            return false;

        var jsc1 = <Atomic.JSComponent>(otherType.objects[0]);
        var jsc2 = <Atomic.JSComponent>(this.objects[0]);

        return jsc1.componentFile == jsc2.componentFile;

    }

    private static Ctor = (() => {

        SelectionInspector.registerEditType("JSComponent", JSComponentEditType);

    })();


}

export = JSComponentEditType;
