
import CreateComponentButton = require("./CreateComponentButton");
import ScriptWidget = require("ui/ScriptWidget");
import EditorEvents = require("editor/EditorEvents");
import SerializableEditType = require("./SerializableEditType");
import SelectionSection = require("./SelectionSection");
import SelectionPrefabWidget = require("./SelectionPrefabWidget");

class NodeSection extends SelectionSection {

    prefabWidget: SelectionPrefabWidget;

    constructor(editType: SerializableEditType) {

        super(editType);

        this.prefabWidget = new SelectionPrefabWidget();
        this.attrLayout.addChild(this.prefabWidget);

    }

}

class ComponentSection extends SelectionSection {

    constructor(editType: SerializableEditType) {

        super(editType);

    }

}

// Node Inspector + Component Inspectors

class SelectionInspector extends ScriptWidget {

    createComponentButton: CreateComponentButton;
    nodeSection: NodeSection;

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

            if (editType.typeName == "Node") {
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


        } else {

            section = new ComponentSection(editType);

        }


        this.mainLayout.removeChild(this.createComponentButton, false);

        this.mainLayout.addChild(section);

        // move the create component button down
        this.mainLayout.addChild(this.createComponentButton);

        this.sections.push(section);

    }

    removeSection(section: SelectionSection) {

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

            if (e.compareTypes(editType)) {
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

    handleSelectionCreateComponent(ev) {

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

    }

    handleSelectionPrefabBreak() {

        if (this.nodes.length != 1)
            return;

        var c = this.getPrefabComponent(this.nodes[0]);
        if (!c)
            return;

        c.breakPrefab();
        this.refresh();

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


    stateChangesInProgress: boolean = false;
    stateChanges: Atomic.Serializable[] = [];

    // ------------------------------------

    static registerEditType(typeName: string, type: typeof SerializableEditType) {

        SelectionInspector._editTypes[typeName] = type;

    }

    private static _editTypes: { [typeName: string]: typeof SerializableEditType } = {};
}

export = SelectionInspector;
