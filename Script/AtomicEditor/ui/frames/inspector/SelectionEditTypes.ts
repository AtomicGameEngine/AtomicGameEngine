//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
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

class CSComponentEditType extends SerializableEditType {

    compareTypes(otherType: SerializableEditType, multiSelect: boolean = false): boolean {

        if (this.typeName != otherType.typeName) {
            return false;
        }

        if (!multiSelect)
            return false;

        var csc1 = <AtomicNETScript.CSComponent>(otherType.objects[0]);
        var csc2 = <AtomicNETScript.CSComponent>(this.objects[0]);

        return csc1.assemblyFile == csc2.assemblyFile && csc1.componentClassName == csc2.componentClassName;

    }

    private static Ctor = (() => {

        SelectionInspector.registerEditType("CSComponent", CSComponentEditType);

    })();


}

export = JSComponentEditType;
