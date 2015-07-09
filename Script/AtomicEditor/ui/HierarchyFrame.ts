
class HierarchyFrame extends Atomic.UIWidget {

    hierList: Atomic.UIListView;

    constructor(parent: Atomic.UIWidget) {

        super();

        this.load("AtomicEditor/editor/ui/hierarchyframe.tb.txt");

        this.gravity = Atomic.UI_GRAVITY_TOP_BOTTOM;

        var hierarchycontainer = parent.getWidget("hierarchycontainer");
        hierarchycontainer.addChild(this);

        hierarchycontainer = this.getWidget("hierarchycontainer");

        var hierList = this.hierList = new Atomic.UIListView();

        hierList.rootList.id = "hierList_";

        hierarchycontainer.addChild(hierList);

        //this.subscribeToEvent("NodeAdded", (data) => this.handleNodeAdded(data));

        this.subscribeToEvent("EditorUpdateHierarchy", (data) => this.handleUpdateHierarchy(data));

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));
        this.subscribeToEvent("EditorActiveSceneChanged", (data) => this.handleActiveSceneChanged(data));

    }

    recursiveAddNode(parentID: number, node: Atomic.Node) {

        //if (node.isTemporary())
        //  return;

        var name = node.name;

        if (!name.length)
            name = "(Anonymous)"

        var childItemID = this.hierList.addChildItem(parentID, name, "Folder.icon", node.id.toString());

        for (var i = 0; i < node.getNumChildren(false); i++) {

            this.recursiveAddNode(childItemID, node.getChildAtIndex(i));

        }

    }

    refresh() {

        this.hierList.deleteAllItems();

        if (!this.scene)
            return;

        var parentID = this.hierList.addRootItem("Scene", "Folder.icon", this.scene.id.toString());

        for (var i = 0; i < this.scene.getNumChildren(false); i++) {

            this.recursiveAddNode(parentID, this.scene.getChildAtIndex(i));

        }

        this.hierList.rootList.value = 0;
        this.hierList.setExpanded(parentID, true);

    }

    handleUpdateHierarchy(data) {

        this.refresh();

    }

    handleActiveSceneChanged(data) {

        this.scene = <Atomic.Scene> data.scene;

        this.refresh();

    }

    handleWidgetEvent(data: Atomic.UIWidgetEvent): boolean {

        if (data.type == Atomic.UI_EVENT_TYPE_CLICK && data.target) {

            var id = data.target.id;

            if (id == "hierList_") {

                var list = <Atomic.UISelectList> data.target;

                var selectedId = Number(list.selectedItemID);
                var node = this.scene.getNode(selectedId);
                this.hierList.rootList.dragObject = new Atomic.UIDragObject(node, node.name.length ? node.name : "(Anonymous)");
                this.sendEvent("EditorActiveNodeChange", { node: node });
                return false;

            }
        }

        return false;
    }

    scene: Atomic.Scene = null;

}

export = HierarchyFrame;
