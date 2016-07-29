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

import EditorUI = require("../../EditorUI");
import CreateComponentButton = require("./CreateComponentButton");
import ScriptWidget = require("ui/ScriptWidget");
import EditorEvents = require("editor/EditorEvents");
import SerializableEditType = require("./SerializableEditType");
import SelectionSection = require("./SelectionSection");
import SelectionPrefabWidget = require("./SelectionPrefabWidget");
import AttributeInfoEdit = require("./AttributeInfoEdit");

class NodeSection extends SelectionSection {

    prefabWidget: SelectionPrefabWidget;

    transformEdits: AttributeInfoEdit[] = [];

    updateDelta: number = 0.0;

    constructor(editType: SerializableEditType) {

        super(editType);

        this.prefabWidget = new SelectionPrefabWidget();
        this.attrLayout.addChild(this.prefabWidget);

        this.transformEdits.push(this.attrEdits["Position"]);
        this.transformEdits.push(this.attrEdits["Rotation"]);
        this.transformEdits.push(this.attrEdits["Scale"]);

        this.subscribeToEvent("Update", (ev) => this.handleUpdate(ev));

    }

    handleUpdate(ev) {

        this.updateDelta -= ev.timeStep;

        if (this.updateDelta < 0.0) {

            this.updateDelta = 0.1;

            Atomic.ui.blockChangedEvents = true;

            for (var i in this.transformEdits) {
                this.transformEdits[i].refresh();
            }

            Atomic.ui.blockChangedEvents = false;

        }


    }


}

class ComponentSection extends SelectionSection {

    constructor(editType: SerializableEditType, inspector: SelectionInspector) {

        super(editType);

        var deleteButton = new Atomic.UIButton();
        deleteButton.text = "Delete Component";
        deleteButton.fontDescription = SelectionSection.fontDesc;

        deleteButton.onClick = () => {

            inspector.onComponentDelete(editType);
            return true;

        };

        var copyButton = new Atomic.UIButton();
        copyButton.text = "Copy Settings";

        copyButton.onClick = () => {

            inspector.onComponentCopy(editType);
            return true;

        };

        var pasteButton = new Atomic.UIButton();
        pasteButton.text = "Paste Settings";

        pasteButton.onClick = () => {
            inspector.onComponentPaste(editType);
            return true;
        };

        this.attrLayout.addChild(deleteButton);
        this.attrLayout.addChild(copyButton);
        this.attrLayout.addChild(pasteButton);

    }

}

class SceneSection extends SelectionSection {

    constructor(editType: SerializableEditType) {

        super(editType);

    }

}

interface AttributeEditResourceChangedEvent {

    attrInfoEdit: AttributeInfoEdit;
    resource: Atomic.Resource;

}

class JSComponentSection extends ComponentSection {

    constructor(editType: SerializableEditType, inspector: SelectionInspector) {

        super(editType, inspector);

        this.hasDynamicAttr = true;

        this.subscribeToEvent(this, "AttributeEditResourceChanged", (ev) => this.handleAttributeEditResourceChanged(ev));

    }

    private handleAttributeEditResourceChanged(ev: AttributeEditResourceChangedEvent) {

        var jsc = <Atomic.JSComponent>this.editType.getFirstObject();

        if (!jsc)
            return;

        var attrInfos = jsc.getAttributes();
        this.updateDynamicAttrInfos(attrInfos);

    }

}

class CSComponentSection extends ComponentSection {

    constructor(editType: SerializableEditType, inspector: SelectionInspector) {

        super(editType, inspector);

        this.hasDynamicAttr = true;

        this.subscribeToEvent(this, "AttributeEditResourceChanged", (ev) => this.handleAttributeEditResourceChanged(ev));

        this.subscribeToEvent("CSComponentAssemblyChanged", (ev) => this.handleCSComponentAssemblyChanged(ev));

    }

    private handleCSComponentAssemblyChanged(ev) {

        var csc = <AtomicNETScript.CSComponent>this.editType.getFirstObject();

        if (!csc)
          return;

        if (csc.assemblyFile == <AtomicNETScript.CSComponentAssembly> ev.resource) {

          var attrInfos = csc.getAttributes();
          this.updateDynamicAttrInfos(attrInfos);

        }

    }

    private handleAttributeEditResourceChanged(ev: AttributeEditResourceChangedEvent) {


        var csc = <AtomicNETScript.CSComponent>this.editType.getFirstObject();

        if (!csc)
            return;

        var attrInfos = csc.getAttributes();
        this.updateDynamicAttrInfos(attrInfos);

    }

}

// Node Inspector + Component Inspectors

class SelectionInspector extends ScriptWidget {

    component: Atomic.Component;

    constructor(sceneEditor: Editor.SceneEditor3D) {

        super();

        this.sceneEditor = sceneEditor;

        var mainLayout = this.mainLayout = new Atomic.UILayout();
        mainLayout.spacing = 4;

        var lp = new Atomic.UILayoutParams();
        lp.width = 304;

        mainLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        mainLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        mainLayout.layoutParams = lp;
        mainLayout.axis = Atomic.UI_AXIS_Y;

        this.addChild(mainLayout);

        var noticeLayout = this.multipleSelectNotice = new Atomic.UILayout();
        noticeLayout.axis = Atomic.UI_AXIS_Y;
        noticeLayout.layoutParams = lp;
        var noticeText = new Atomic.UITextField();
        noticeText.textAlign = Atomic.UI_TEXT_ALIGN_CENTER;
        noticeText.skinBg = "InspectorTextAttrName";
        noticeText.text = "Multiple Selection - Some components are hidden";
        noticeText.fontDescription = SelectionSection.fontDesc;
        noticeText.gravity = Atomic.UI_GRAVITY_LEFT_RIGHT;
        noticeText.layoutParams = lp;
        noticeLayout.addChild(noticeText);
        noticeLayout.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
        mainLayout.addChild(noticeLayout);

        this.createComponentButton = new CreateComponentButton();
        mainLayout.addChild(this.createComponentButton);

        this.subscribeToEvent(sceneEditor.scene, "SceneEditStateChangesBegin", (data) => this.handleSceneEditStateChangesBeginEvent());
        this.subscribeToEvent("SceneEditStateChange", (data) => this.handleSceneEditStateChangeEvent(data));
        this.subscribeToEvent(sceneEditor.scene, "SceneEditStateChangesEnd", (data) => this.handleSceneEditStateChangesEndEvent());

        this.subscribeToEvent(sceneEditor.scene, "SceneEditNodeRemoved", (ev: Editor.SceneEditNodeRemovedEvent) => this.handleSceneEditNodeRemoved(ev));
        this.subscribeToEvent(sceneEditor.scene, "SceneEditComponentAddedRemoved", (ev) => this.handleSceneEditComponentAddedRemovedEvent(ev));

        this.subscribeToEvent(this.createComponentButton, "SelectionCreateComponent", (data) => this.handleSelectionCreateComponent(data));

    }

    pruneSections() {

        var remove: SelectionSection[] = [];

        for (var i in this.sections) {

            var section = this.sections[i];

            var editType = section.editType;

            if (editType.typeName == "Node") {
                continue;
            }

            if (editType.typeName == "Scene") {
                var gotone = false;
                for (var j in this.nodes) {
                    if (this.nodes[j].typeName == "Scene") {
                        gotone = true;
                        break;
                    }
                }
                if (gotone)
                    continue;
            }

            if (!editType.nodes.length) {

                remove.push(section);

            }

        }

        if (remove.length) {

            for (var i in remove) {

                var section = remove[i];
                this.removeSection(section);

            }

            this.suppressSections();
        }

    }

    suppressSections() {

        this.multipleSelectNotice.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;

        for (var i in this.sections) {

            var section = this.sections[i];
            var editType = section.editType;

            if (editType.typeName == "Node" || editType.typeName == "Scene") {
                continue;
            }

            var suppressed = false;

            for (var j in this.nodes) {
                if (editType.nodes.indexOf(this.nodes[j]) == -1) {
                    suppressed = true;
                    break;
                }
            }

            if (suppressed)
                this.multipleSelectNotice.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;

            section.suppress(suppressed);

        }

    }

    refresh() {

        Atomic.ui.blockChangedEvents = true;

        this.pruneSections();
        this.suppressSections();

        for (var i in this.sections) {

            this.sections[i].refresh();

        }

        if (this.nodeSection) {
            this.nodeSection.prefabWidget.updateSelection(this.nodes);
        }

        Atomic.ui.blockChangedEvents = false;

    }

    addSection(editType: SerializableEditType) {

        var section: SelectionSection;

        if (editType.typeName == "Node") {

            this.nodeSection = new NodeSection(editType);
            section = this.nodeSection;

        } else if (editType.typeName == "Scene") {

            section = new SceneSection(editType);

        } else if (editType.typeName == "JSComponent") {

            section = new JSComponentSection(editType, this);

        } else if (editType.typeName == "CSComponent") {

            section = new CSComponentSection(editType, this);
        }
        else {
            section = new ComponentSection(editType, this);
        }

        section.value = SelectionInspector.sectionStates[editType.typeName] ? 1 : 0;

        this.mainLayout.removeChild(this.createComponentButton, false);
        this.mainLayout.removeChild(this.multipleSelectNotice, false);

        // sort it in alphabetically

        this.sections.push(section);

        this.sections.sort(function(a, b) {

            if (a.editType.typeName == "Node" && b.editType.typeName == "Scene")
                return -1;

            if (a.editType.typeName == "Scene" && b.editType.typeName == "Node")
                return 1;

            if (a.editType.typeName == "Node" || a.editType.typeName == "Scene")
                return -1;

            if (b.editType.typeName == "Node" || b.editType.typeName == "Scene")
                return 1;

            return a.editType.typeName.localeCompare(b.editType.typeName);
        });

        var idx = this.sections.indexOf(section);

        if (idx == 0) {

            if (this.sections.length == 1) {
                this.mainLayout.addChild(section);
            } else {
                this.mainLayout.addChildBefore(section, this.sections[1]);
            }
        }
        else if (idx == this.sections.length - 1) {

            this.mainLayout.addChild(section);
        }
        else {
            this.mainLayout.addChildAfter(section, this.sections[idx - 1]);
        }

        // move the create component button down

        this.mainLayout.addChild(this.multipleSelectNotice);
        this.mainLayout.addChild(this.createComponentButton);

    }

    removeSection(section: SelectionSection) {

        SelectionInspector.sectionStates[section.editType.typeName] = section.value ? true : false;
        var index = this.sections.indexOf(section);
        this.sections.splice(index, 1);
        this.mainLayout.removeChild(section);

    }

    removeSerializable(serial: Atomic.Serializable) {

        for (var i in this.sections) {

            var section = this.sections[i];

            var e = section.editType;

            var index = e.objects.indexOf(serial);

            if (index != -1) {

                e.objects.splice(index, 1);

            }

            if (serial.typeName == "Node") {

                index = e.nodes.indexOf(<Atomic.Node>serial);

                if (index != -1) {

                    e.nodes.splice(index, 1);

                }
            }

        }

    }

    addSerializable(serial: Atomic.Serializable): SerializableEditType {

        var editType = this.getEditType(serial);

        // does it already exist?
        for (var i in this.sections) {

            var section = this.sections[i];

            var e = section.editType;

            if (e.compareTypes(editType, this.nodes.length > 1)) {
                e.addSerializable(serial);
                return e;
            }

        }

        this.addSection(editType);

        return editType;

    }

    getEditType(serial: Atomic.Serializable): SerializableEditType {

        var typeName = serial.typeName;


        if (SelectionInspector._editTypes[typeName]) {
            return new SelectionInspector._editTypes[typeName](serial);
        }

        return new SerializableEditType(serial);

    }

    addNode(node: Atomic.Node) {

        var index = this.nodes.indexOf(node);

        if (index == -1) {
            this.nodes.push(node);
            this.addSerializable(node);
            var components = node.getComponents();
            for (var i in components) {

                if (this.filterComponent(components[i]))
                    continue;

                var editType = this.addSerializable(components[i]);
                editType.addNode(node);
            }
            this.refresh();
        }
    }

    removeNode(node: Atomic.Node) {

        var index = this.nodes.indexOf(node);

        if (index != -1) {

            this.nodes.splice(index, 1);
            this.removeSerializable(node);
            var components = node.getComponents();
            for (var i in components) {

                if (this.filterComponent(components[i]))
                    continue;

                this.removeSerializable(components[i]);
            }

            this.refresh();
        }

        // save node section state
        if (!this.nodes.length && this.nodeSection)
            SelectionInspector.sectionStates["Node"] = this.nodeSection.value ? true : false;

    }

    handleSceneEditStateChangesBeginEvent() {

        this.stateChangesInProgress = true;

    }

    handleSceneEditNodeRemoved(ev: Editor.SceneEditNodeRemovedEvent) {

        this.removeNode(ev.node);

    }

    handleSceneEditComponentAddedRemovedEvent(ev: Editor.SceneEditComponentAddedRemovedEvent) {

        if (this.filterComponent(ev.component)) {
            // still refresh as may affect UI (for example PrefabComponents)
            this.refresh();
            return;
        }

        var editType;
        if (!ev.removed) {

            editType = this.addSerializable(ev.component);
            editType.addNode(ev.node);

        } else {

            for (var i in this.sections) {

                var section = this.sections[i];
                editType = section.editType;

                var index = editType.objects.indexOf(ev.component);
                if (index != -1) {

                    editType.objects.splice(index, 1);
                    index = editType.nodes.indexOf(ev.node);
                    if (index != -1) {
                        editType.nodes.splice(index, 1);
                    }
                    break;

                }

            }

        }

        this.refresh();

    }

    onComponentDelete(editType: SerializableEditType) {

        var removed: Atomic.Component[] = [];

        for (var i in editType.objects) {

            var c = <Atomic.Component>editType.objects[i];
            removed.push(c);

        }

        for (var i in removed) {

            var c = removed[i];

            var node = c.node;
            c.remove();

            this.removeSerializable(removed[i]);

            var index = editType.nodes.indexOf(node);
            if (index != -1) {
                editType.nodes.splice(index, 1);
            }

        }

        if (removed.length) {

            this.sceneEditor.scene.sendEvent("SceneEditEnd");
            this.refresh();

        }

    }

    onComponentCopy(editType: SerializableEditType) {

        var copy: Atomic.Component[] = [];

        for (var i in editType.objects) {

            var c = <Atomic.Component>editType.objects[i];
            copy.push(c);

        }

        for (var i in copy) {

            var c = copy[i];

            this.component = c;

            this.sceneEditor.scene.sendEvent("SceneEditComponentCopy", { component: this.component });
            this.refresh();

        }
    }

    onComponentPaste(editType: SerializableEditType) {
        var paste: Atomic.Component[] = [];

        for (var i in editType.objects) {

            var c = <Atomic.Component>editType.objects[i];
            paste.push(c);

        }

        for (var i in paste) {

            var c = paste[i];

            this.component = c;

            this.sceneEditor.scene.sendEvent("SceneEditComponentPaste", { component: this.component });
            this.refresh();
        }

    }

    handleSelectionCreateComponent(ev) {

        var valid = true;

        if (ev.componentTypeName != "JSComponent" && ev.componentTypeName != "CSComponent") {

            for (var i in this.nodes) {

                var node = this.nodes[i];
                if (node.getComponent(ev.componentTypeName, false)) {
                    valid = false;
                    break;
                }
            }
        }

        if (!valid) {

            EditorUI.showModalError("Component Create", "Unable to create component, a node with an existing " + ev.componentTypeName + " component is selected");
            return;

        }

        for (var i in this.nodes) {

            var node = this.nodes[i];

            var c = node.createComponent(ev.componentTypeName);

            if (!c) {
                console.log("ERROR: unable to create component ", ev.componentTypeName);
                return;
            }

            var editType = this.addSerializable(c);
            editType.addNode(node);

            for (var i in this.sections) {
                if (this.sections[i].editType == editType) {
                    this.sections[i].value = 1;
                    break;
                }

            }

        }

        this.refresh();

    }


    handleSceneEditStateChangeEvent(ev: Editor.SceneEditStateChangeEvent) {

        if (!this.stateChangesInProgress)
            return;

        if (this.stateChanges.indexOf(ev.serializable) == -1) {
            this.stateChanges.push(ev.serializable);
        }

    }

    getPrefabComponent(node: Atomic.Node): Atomic.PrefabComponent {

        if (node.getComponent("PrefabComponent"))
            return <Atomic.PrefabComponent>node.getComponent("PrefabComponent");

        if (node.parent)
            return this.getPrefabComponent(node.parent);

        return null;

    }

    filterComponent(component: Atomic.Component): boolean {

        if (component.typeName == "PrefabComponent") {
            return true;
        }

        return false;

    }

    handleSceneEditStateChangesEndEvent() {

        Atomic.ui.blockChangedEvents = true;

        var sections: SelectionSection[] = [];

        for (var i in this.stateChanges) {

            var serial = this.stateChanges[i];

            for (var j in this.sections) {

                var section = this.sections[j];

                if (sections.indexOf(section) != -1)
                    continue;

                if (section.editType.objects.indexOf(serial) != -1) {

                    sections.push(section);

                    if (section.hasDynamicAttr) {

                        var object = section.editType.getFirstObject();
                        if (object) {
                            var attrInfos = object.getAttributes();
                            section.updateDynamicAttrInfos(attrInfos);
                        }
                    }

                    section.refresh();
                }

            }

        }

        Atomic.ui.blockChangedEvents = false;
        this.stateChanges = [];
        this.stateChangesInProgress = false;

    }


    mainLayout: Atomic.UILayout;

    multipleSelectNotice: Atomic.UILayout;

    sceneEditor: Editor.SceneEditor3D;
    nodes: Atomic.Node[] = [];
    sections: SelectionSection[] = [];

    createComponentButton: CreateComponentButton;
    nodeSection: NodeSection;

    stateChangesInProgress: boolean = false;
    stateChanges: Atomic.Serializable[] = [];

    // ------------------------------------

    static registerEditType(typeName: string, type: typeof SerializableEditType) {

        SelectionInspector._editTypes[typeName] = type;

    }

    private static sectionStates: { [typeName: string]: boolean } = {};
    private static _editTypes: { [typeName: string]: typeof SerializableEditType } = {};

    private static Ctor = (() => {

        SelectionInspector.sectionStates["Node"] = true;

    })();

}

export = SelectionInspector;
