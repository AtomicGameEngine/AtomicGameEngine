
import HierarchyFrameMenu = require("./menus/HierarchyFrameMenu");
import MenuItemSources = require("./menus/MenuItemSources");
import EditorEvents = require("editor/EditorEvents");

var IconTemporary = "ComponentBitmap";

class HierarchyFrame extends Atomic.UIWidget {

    scene: Atomic.Scene = null;
    hierList: Atomic.UIListView;
    menu: HierarchyFrameMenu;
    nodeIDToItemID = {};

    constructor(parent: Atomic.UIWidget) {

        super();

        this.menu = new HierarchyFrameMenu();

        this.load("AtomicEditor/editor/ui/hierarchyframe.tb.txt");

        this.gravity = Atomic.UI_GRAVITY_TOP_BOTTOM;

        var hierarchycontainer = parent.getWidget("hierarchycontainer");
        hierarchycontainer.addChild(this);

        hierarchycontainer = this.getWidget("hierarchycontainer");

        var hierList = this.hierList = new Atomic.UIListView();

        hierList.rootList.id = "hierList_";

        hierarchycontainer.addChild(hierList);

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

        this.subscribeToEvent(EditorEvents.ActiveNodeChange, (data) => {

            if (data.node)
                this.hierList.selectItemByID(data.node.id.toString());

        });

        this.subscribeToEvent(EditorEvents.ActiveSceneChange, (data) => this.handleActiveSceneChanged(data));

        this.subscribeToEvent(EditorEvents.SceneClosed, (ev: EditorEvents.SceneClosedEvent) => {

            if (ev.scene == this.scene) {

                this.unsubscribeFromEvents(this.scene);
                this.scene = null;
                this.populate();

            }

        });

        this.subscribeToEvent("ComponentAdded", (ev: Atomic.ComponentAddedEvent) => {

            if (!ev.component || ev.component.typeName != "PrefabComponent") return;

            var node = ev.node;

            var itemID = this.nodeIDToItemID[node.id];

            if (itemID) {

                this.hierList.setItemTextSkin(node.id.toString(), "HierarchyPrefabText");

            }

        });

        this.subscribeToEvent("ComponentRemoved", (ev: Atomic.ComponentRemovedEvent) => {

            if (!ev.component || ev.component.typeName != "PrefabComponent") return;

            var node = ev.node;

            var itemID = this.nodeIDToItemID[node.id];

            if (itemID) {

                this.hierList.setItemTextSkin(node.id.toString(), "Folder");
            }

        });

        this.subscribeToEvent("TemporaryChanged", (ev: Atomic.TemporaryChangedEvent) => {

            // this can happen on a temporary status change on a non-scripted class instance
            if (!ev.serializable) {
                return;
            }

            if (ev.serializable.typeName == "Node") {

                var node = <Atomic.Node> ev.serializable;

                var itemID = this.nodeIDToItemID[node.id];

                if (itemID) {

                    this.hierList.setItemIcon(node.id.toString(), node.isTemporary() ? IconTemporary : "");

                }

            }

        });

    }

    handleNodeAdded(ev: Atomic.NodeAddedEvent) {

        var node = ev.node;

        if (this.filterNode(node))
            return;

        if (!node.parent || node.scene != this.scene)
            return;

        var parentID = this.nodeIDToItemID[node.parent.id];

        var childItemID = this.recursiveAddNode(parentID, node);

        this.nodeIDToItemID[node.id] = childItemID;

    }

    handleNodeRemoved(ev: Atomic.NodeRemovedEvent) {

        // on close
        if (!this.scene)
            return;

        var node = ev.node;

        if (this.filterNode(node))
            return;

        delete this.nodeIDToItemID[node.id];

        if (!node.parent || node.scene != this.scene)
            return;

        this.hierList.deleteItemByID(node.id.toString());

        var selectedId = Number(this.hierList.rootList.selectedItemID);
        var selectedNode = this.scene.getNode(selectedId);
        if (selectedNode == node) {

            this.sendEvent(EditorEvents.ActiveNodeChange, { node: ev.parent ? ev.parent : this.scene });

        }

    }

    handleActiveSceneChanged(data) {

        if (this.scene)
            this.unsubscribeFromEvents(this.scene);

        // clear selected node
        this.sendEvent(EditorEvents.ActiveNodeChange, { node: null });

        this.scene = <Atomic.Scene> data.scene;

        this.populate();

        if (this.scene) {

            this.subscribeToEvent(this.scene, "NodeAdded", (ev: Atomic.NodeAddedEvent) => this.handleNodeAdded(ev));
            this.subscribeToEvent(this.scene, "NodeRemoved", (ev: Atomic.NodeRemovedEvent) => this.handleNodeRemoved(ev));
            this.subscribeToEvent(this.scene, "NodeNameChanged", (ev: Atomic.NodeNameChangedEvent) => {

                this.hierList.setItemText(ev.node.id.toString(), ev.node.name);

            });

        }

    }

    handleWidgetEvent(data: Atomic.UIWidgetEvent): boolean {

        if (data.type == Atomic.UI_EVENT_TYPE_KEY_UP) {

            if (data.key == Atomic.KEY_DELETE || data.key == Atomic.KEY_BACKSPACE) {

                var selectedId = Number(this.hierList.rootList.selectedItemID);

                var node = this.scene.getNode(selectedId);
                if (node) {

                    node.removeAllComponents();
                    node.remove();

                }

            }

        }

        if (data.type == Atomic.UI_EVENT_TYPE_POINTER_DOWN) {

            if (data.target == this.hierList.rootList) {

                var node = this.scene.getNode(Number(data.refid));

                if (node) {

                    // set the widget's drag object
                    var dragObject = new Atomic.UIDragObject(node, node.name.length ? "Node: " + node.name : "Node: (Anonymous)");
                    this.hierList.rootList.dragObject = dragObject;

                    // handle dropping on hierarchy, moving node
                    this.subscribeToEvent(dragObject, "DragEnded", (ev: Atomic.DragEndedEvent) => {

                        var dragNode = <Atomic.Node> ev.dragObject.object;

                        var dropNode: Atomic.Node = this.scene.getNode(Number(this.hierList.hoverItemID));

                        if (!dropNode) {
                            return;
                        }

                        // can't drop on self
                        if (dragNode == dropNode) {
                            return;
                        }

                        // check if dropping on child of ourselves
                        var parent = dropNode.parent;

                        while (parent) {

                            if (parent == dragNode) {
                                return;
                            }

                            parent = parent.parent;

                        }

                        // move it
                        dropNode.addChild(dragNode);

                    });

                }

            }

        }

        if (data.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = data.target.id;

            if (id == "create popup") {

                var selectedId = Number(this.hierList.rootList.selectedItemID);
                var node = this.scene.getNode(selectedId);

                if (this.menu.handlePopupMenu(data.target, data.refid, node))
                    return true;

            }

            // create
            if (id == "menu create") {
                if (!ToolCore.toolSystem.project) return;
                var src = MenuItemSources.getMenuItemSource("hierarchy create items");
                var menu = new Atomic.UIMenuWindow(data.target, "create popup");
                menu.show(src);
                return true;

            }


            if (id == "hierList_") {

                var list = <Atomic.UISelectList> data.target;

                var selectedId = Number(list.selectedItemID);
                var node = this.scene.getNode(selectedId);

                if (node) {


                    this.sendEvent("EditorActiveNodeChange", { node: node });

                }

                return false;

            }
        }

        return false;
    }

    filterNode(node: Atomic.Node): boolean {

        if (!node) return false;

        if (node.name == "__atomic_sceneview3d_camera") return true;

        return false;

    }

    recursiveAddNode(parentID: number, node: Atomic.Node): number {

        if (this.filterNode(node))
            return;

        var name = node.name;

        if (!name.length)
            name = "(Anonymous)"

        var icon = "";

        if (node.isTemporary())
            icon = IconTemporary;

        var childItemID = this.hierList.addChildItem(parentID, name, icon, node.id.toString());

        if (node.getComponent("PrefabComponent")) {

            this.hierList.setItemTextSkin(node.id.toString(), "HierarchyPrefabText");

        }

        this.nodeIDToItemID[node.id] = childItemID;

        for (var i = 0; i < node.getNumChildren(false); i++) {

            this.recursiveAddNode(childItemID, node.getChildAtIndex(i));

        }

        return childItemID;

    }

    populate() {

        this.nodeIDToItemID = {};
        this.hierList.deleteAllItems();

        if (!this.scene)
            return;

        var parentID = this.hierList.addRootItem("Scene", "", this.scene.id.toString());

        this.nodeIDToItemID[this.scene.id] = parentID;

        for (var i = 0; i < this.scene.getNumChildren(false); i++) {

            this.recursiveAddNode(parentID, this.scene.getChildAtIndex(i));

        }

        this.hierList.rootList.value = 0;
        this.hierList.setExpanded(parentID, true);

    }

}

export = HierarchyFrame;
