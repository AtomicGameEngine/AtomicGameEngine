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

class SerializableEditType {

    constructor(serial: Atomic.Serializable) {

        this.typeName = serial.typeName;
        this.attrInfos = serial.getAttributes();
        this.addSerializable(serial);
    }

    addSerializable(serial: Atomic.Serializable) {

        this.objects.push(serial);

    }

    getUniformValue(attrInfo: Atomic.AttributeInfo, index: number = -1): boolean {

        if (this.objects.length <= 1)
            return true;

        var value: any;

        for (var i = 0; i < this.objects.length; i++) {

            var object = this.objects[i];

            if (i == 0) {

                value = object.getAttribute(attrInfo.name);
                if (index >= 0) {

                    if (attrInfo.type == Atomic.VAR_RESOURCEREFLIST) {

                        value = value.resources[index];

                    } else {
                        value = value[index];
                    }
                }

            } else {

                var value2 = object.getAttribute(attrInfo.name);
                if (index >= 0) {
                    if (attrInfo.type == Atomic.VAR_RESOURCEREFLIST) {

                        value2 = value2.resources[index];

                    } else {
                        value2 = value2[index];
                    }
                }

                if (value != value2)
                    return false;

            }

        }

        return true;


    }

    onAttributeInfoEdited(attrInfo: Atomic.AttributeInfo, value: any, index: number = -1, genEdit: boolean = true) {

        for (var i in this.objects) {

            var object = this.objects[i];

            if (index >= 0) {

                var idxValue = object.getAttribute(attrInfo.name);

                if (attrInfo.type == Atomic.VAR_RESOURCEREFLIST) {

                    idxValue.resources[index] = value;
                    object.setAttribute(attrInfo.name, idxValue);

                } else {

                    idxValue[index] = value;
                    object.setAttribute(attrInfo.name, idxValue);

                }

            } else {

                object.setAttribute(attrInfo.name, value);

            }

        }

        if (!genEdit)
            return;

        var node: Atomic.Node = null;
        if (this.nodes.length) {
            node = this.nodes[0];
        } else if (this.objects.length && this.objects[0].typeName == "Node") {
            node = <Atomic.Node>this.objects[0];
        }

        if (node)
            node.scene.sendEvent("SceneEditEnd");

    }

    compareTypes(otherType: SerializableEditType, multiSelect:boolean = false): boolean {

        return this.typeName == otherType.typeName;

    }

    addNode(node: Atomic.Node) {

        if (this.nodes.indexOf(node) == -1) {
            this.nodes.push(node);
        }

    }

    getFirstObject(): Atomic.Serializable {

        if (this.objects.length) {
            return this.objects[0];
        }

        return null;

    }

    typeName: string;
    attrInfos: Atomic.AttributeInfo[];

    nodes: Atomic.Node[] = [];
    objects: Atomic.Serializable[] = [];

}

export = SerializableEditType;
