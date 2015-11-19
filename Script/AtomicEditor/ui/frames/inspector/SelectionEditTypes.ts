

import SerializableEditType = require("./SerializableEditType");
import SelectionInspector = require("./SelectionInspector");

class JSComponentEditType extends SerializableEditType {

    compareTypes(otherType: SerializableEditType): boolean {

        if (this.typeName != otherType.typeName) {
            return false;
        }

        var jsc1 = <Atomic.JSComponent>(otherType.objects[0]);
        var jsc2 = <Atomic.JSComponent>(this.objects[0]);

        return jsc1.componentFile == jsc2.componentFile;

    }

    private static Ctor = (() => {

        SelectionInspector.registerEditType("JSComponent", JSComponentEditType);

    })();


}

export = JSComponentEditType;
