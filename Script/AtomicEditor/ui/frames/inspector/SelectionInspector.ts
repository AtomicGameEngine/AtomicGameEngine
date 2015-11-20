
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

        }

        this.attrLayout.addChild(deleteButton);;

    }

}

class SceneSection extends SelectionSection {

    constructor(editType: SerializableEditType) {

        super(editType);

    }

}

// Node Inspector + Component Inspectors

class SelectionInspector extends ScriptWidget {

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

        this.createComponentButton = new CreateComponentButton();
        mainLayout.addChild(this.createComponentButton);

        this.subscribeToEvent(sceneEditor.scene, "SceneEditStateChangesBegin", (data) => this.handleSceneEditStateChangesBeginEvent());
        this.subscribeToEvent("SceneEditStateChange", (data) => this.handleSceneEditStateChangeEvent(data));
        this.subscribeToEvent(sceneEditor.scene, "SceneEditStateChangesEnd", (data) => this.handleSceneEditStateChangesEndEvent());

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

            this.subscribeToEvent(this.nodeSection.prefabWidget, "SelectionPrefabSave", (data) => this.handleSelectionPrefabSave());
            this.subscribeToEvent(this.nodeSection.prefabWidget, "SelectionPrefabUndo", (data) => this.handleSelectionPrefabUndo());
            this.subscribeToEvent(this.nodeSection.prefabWidget, "SelectionPrefabBreak", (data) => this.handleSelectionPrefabBreak());


        } else if (editType.typeName == "Scene") {

            section = new SceneSection(editType);

        } else {

            section = new ComponentSection(editType, this);

        }

        section.value = SelectionInspector.sectionStates[editType.typeName] ? 1 : 0;

        this.mainLayout.removeChild(this.createComponentButton, false);

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
                this.removeSerializable(components[i]);
            }

            this.refresh();
        }

    }

    handleSceneEditStateChangesBeginEvent() {

        this.stateChangesInProgress = true;

    }

    handleSceneEditComponentAddedRemovedEvent(ev: Editor.SceneEditComponentAddedRemovedEvent) {

        if (!ev.removed) {

            editType = this.addSerializable(ev.component);
            editType.addNode(ev.node);

        } else {

            for (var i in this.sections) {

                var section = this.sections[i];
                var editType = section.editType;

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

    handleSelectionCreateComponent(ev) {

        var valid = true;

        if (ev.componentTypeName != "JSComponent") {

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

    handleSelectionPrefabSave() {

        if (this.nodes.length != 1)
            return;

        var c = this.getPrefabComponent(this.nodes[0]);
        if (!c)
            return;

        c.savePrefab();

        var node = this.nodes[0];
        this.removeNode(node);
        this.addNode(node);

    }

    handleSelectionPrefabBreak() {

        if (this.nodes.length != 1)
            return;

        var c = this.getPrefabComponent(this.nodes[0]);
        if (!c)
            return;

        c.breakPrefab();

        var node = this.nodes[0];
        this.removeNode(node);
        this.addNode(node);

    }

    handleSelectionPrefabUndo() {

        if (this.nodes.length != 1)
            return;

        var c = this.getPrefabComponent(this.nodes[0]);
        if (!c)
            return;

        c.undoPrefab();

        var node = this.nodes[0];
        this.removeNode(node);
        this.addNode(node);

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
                    section.refresh();
                }

            }

        }

        Atomic.ui.blockChangedEvents = false;
        this.stateChanges = [];
        this.stateChangesInProgress = false;

    }


    mainLayout: Atomic.UILayout;

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
