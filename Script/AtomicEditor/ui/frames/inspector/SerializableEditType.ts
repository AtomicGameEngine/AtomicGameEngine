//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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
