//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import HierarchyFrameMenu = require("./menus/HierarchyFrameMenu");
import MenuItemSources = require("./menus/MenuItemSources");
import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");

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

        // handle dropping on hierarchy, moving node, dropping prefabs, etc
        this.subscribeToEvent(this.hierList.rootList, "DragEnded", (data) => this.handleDragEnded(data));

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

                var node = <Atomic.Node>ev.serializable;

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

        this.scene = <Atomic.Scene>data.scene;

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

            if (data.key == Atomic.KEY_DOWN || data.key == Atomic.KEY_UP || data.key == Atomic.KEY_LEFT || data.key == Atomic.KEY_RIGHT) {
                var selectedId = Number(this.hierList.selectedItemID);
                var node = this.scene.getNode(selectedId);

                if (node) {

                    this.sendEvent("EditorActiveNodeChange", { node: node });

                }
            }
            if (data.key == Atomic.KEY_RIGHT) {
                var selectedId = Number(this.hierList.selectedItemID);
                var itemNodeId = this.nodeIDToItemID[selectedId];

                if (!this.hierList.getExpanded(itemNodeId) && this.hierList.getExpandable(itemNodeId)) {
                    this.hierList.setExpanded(itemNodeId, true);
                    this.hierList.rootList.invalidateList();
                } else {
                    this.hierList.rootList.selectNextItem();
                }

            } else if (data.key == Atomic.KEY_LEFT) {
                var selectedId = Number(this.hierList.selectedItemID);
                var itemNodeId = this.nodeIDToItemID[selectedId];

                if (this.hierList.getExpanded(itemNodeId)) {
                    this.hierList.setExpanded(itemNodeId, false);
                    this.hierList.rootList.invalidateList();
                } else {
                    var node = this.scene.getNode(selectedId);
                    var parentNode = node.getParent();
                    if (parentNode) {
                        this.hierList.selectItemByID(parentNode.id.toString());
                    }
                }

            }

            // node deletion
            if (data.key == Atomic.KEY_DELETE || data.key == Atomic.KEY_BACKSPACE) {

                var selectedId = Number(this.hierList.rootList.selectedItemID);

                var node = this.scene.getNode(selectedId);
                if (node) {
                    this.scene.sendEvent("SceneEditNodeAddedRemoved", { scene:this.scene, node:node, added:false});
                    node.remove();

                }

            }

        } else if (data.type == Atomic.UI_EVENT_TYPE_POINTER_DOWN) {

            if (data.target == this.hierList.rootList) {

                var node = this.scene.getNode(Number(data.refid));

                if (node) {

                    // set the widget's drag object
                    var dragObject = new Atomic.UIDragObject(node, node.name.length ? "Node: " + node.name : "Node: (Anonymous)");
                    this.hierList.rootList.dragObject = dragObject;

                }

            }

        } else if (data.type == Atomic.UI_EVENT_TYPE_CLICK) {

            if (this.menu.handleNodeContextMenu(data.target, data.refid)) {
                return true;
            }

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

                var list = <Atomic.UISelectList>data.target;

                var selectedId = Number(list.selectedItemID);
                var node = this.scene.getNode(selectedId);

                if (node) {

                    this.sendEvent("EditorActiveNodeChange", { node: node });

                }

                return false;

            }
        } else if (data.type == Atomic.UI_EVENT_TYPE_RIGHT_POINTER_UP) {

            var id = data.target.id;
            var db = ToolCore.getAssetDatabase();
            var node: Atomic.Node;

            if (id == "hierList_")
                node = this.scene.getNode(Number(this.hierList.hoverItemID));
            else
                node = this.scene.getNode(Number(id));

            if (node) {

                this.menu.createNodeContextMenu(this, node, data.x, data.y);

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

    handleDragEnded(ev: Atomic.DragEndedEvent) {

        var typeName = ev.dragObject.object.typeName;

        var dropNode: Atomic.Node = this.scene.getNode(Number(this.hierList.hoverItemID));

        if (!dropNode) return;

        if (typeName == "Node") {

            var dragNode = <Atomic.Node>ev.dragObject.object;

            if (dragNode.scene != this.scene) {
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

        } else if (typeName == "Asset") {

            var asset = <ToolCore.Asset>ev.dragObject.object;
            asset.instantiateNode(dropNode, asset.name);

        }

    }

}

export = HierarchyFrame;
