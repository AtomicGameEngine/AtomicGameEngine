import MainFrameMenu = require("./MainFrameMenu");
import ProjectFrame = require("./ProjectFrame");
import ResourceFrame = require("./ResourceFrame");
import InspectorFrame = require("./inspector/InspectorFrame");
import HierarchyFrame = require("./HierarchyFrame");
import MainToolbar = require("./MainToolbar");

import MessageModal = require("./modal/MessageModal");
import UIEvents = require("./UIEvents");

import ScriptWidget = require("./ScriptWidget");

class MainFrame extends ScriptWidget {

    projectframe: ProjectFrame;
    resourceframe: ResourceFrame;
    inspectorframe: InspectorFrame;
    hierarchyFrame: HierarchyFrame;
    inspectorlayout: Atomic.UILayout;
    mainToolbar: MainToolbar;
    menu: MainFrameMenu;

    private messagemodal: MessageModal.MessageModal = new MessageModal.MessageModal();

    constructor() {

        super();

        this.load("AtomicEditor/editor/ui/mainframe.tb.txt");

        this.inspectorlayout = <Atomic.UILayout> this.getWidget("inspectorlayout");

        this.getWidget("consolecontainer").visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;

        this.inspectorframe = new InspectorFrame();
        this.inspectorlayout.addChild(this.inspectorframe);

        this.projectframe = new ProjectFrame(this);
        this.hierarchyFrame = new HierarchyFrame(this);

        this.resourceframe = new ResourceFrame(this);

        this.mainToolbar = new MainToolbar(this.getWidget("maintoolbarcontainer"));

        this.menu = new MainFrameMenu();

        this.showInspectorFrame(true);

        this.subscribeToEvent(UIEvents.ResourceEditorChanged, (data) => this.handleResourceEditorChanged(data));

    }

    showInspectorFrame(show: boolean) {

        return;

        if (show) {

            this.inspectorlayout.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;
            this.inspectorframe.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;

        } else {

            this.inspectorframe.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
            this.inspectorlayout.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;

        }


    }

    onEventClick(target: Atomic.UIWidget, refid: string): boolean {

        if (this.menu.handlePopupMenu(target, refid))
            return true;

        var src = this.menu.getMenuItemSource(target.id);

        if (src) {

            var menu = new Atomic.UIMenuWindow(target, target.id + " popup");
            menu.show(src);
            return true;

        }

        return false;

    }

    handleResourceEditorChanged(data): void {

        var editor = <Editor.ResourceEditor> data.editor;

        if (editor) {

            this.showInspectorFrame(editor.requiresInspector());

        } else {

            this.showInspectorFrame(false);

        }

    }


}

export = MainFrame;
