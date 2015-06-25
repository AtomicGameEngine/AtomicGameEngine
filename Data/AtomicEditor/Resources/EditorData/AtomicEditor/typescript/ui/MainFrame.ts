import menubar = require("./MainFrameMenu");
import ProjectFrame = require("./ProjectFrame");
import ResourceFrame = require("./ResourceFrame");
import InspectorFrame = require("./inspector/InspectorFrame");
import HierarchyFrame = require("./HierarchyFrame");

import MessageModal = require("./modal/MessageModal");
import UIEvents = require("./UIEvents");

import ScriptWidget = require("./ScriptWidget");

var UI = Atomic.UI;

class MainFrame extends ScriptWidget {

    projectframe: ProjectFrame;
    resourceframe: ResourceFrame;
    inspectorframe: InspectorFrame;
    hierarchyFrame: HierarchyFrame;

    inspectorlayout: Atomic.UILayout;

    private messagemodal: MessageModal.MessageModal = new MessageModal.MessageModal();

    constructor() {

        super();

        this.load("AtomicEditor/editor/ui/mainframe.tb.txt");

        this.inspectorlayout = <Atomic.UILayout> this.getWidget("inspectorlayout");

        this.inspectorframe = new InspectorFrame();
        this.inspectorlayout.addChild(this.inspectorframe);

        this.projectframe = new ProjectFrame(this);
        this.hierarchyFrame = new HierarchyFrame(this);

        this.resourceframe = new ResourceFrame(this);

        this.showInspectorFrame(true);

        this.subscribeToEvent(UIEvents.ResourceEditorChanged, (data) => this.handleResourceEditorChanged(data));

    }

    showInspectorFrame(show: boolean) {

        return;

        if (show) {

            this.inspectorlayout.visibility = UI.VISIBILITY_VISIBLE;
            this.inspectorframe.visibility = UI.VISIBILITY_VISIBLE;

        } else {

            this.inspectorframe.visibility = UI.VISIBILITY_GONE;
            this.inspectorlayout.visibility = UI.VISIBILITY_GONE;

        }


    }

    onEventClick(target: Atomic.UIWidget, refid: string): boolean {

        if (this.handlePopupMenu(target, refid))
            return true;

        var src = menubar.getMenuItemSource(target.id);

        if (src) {

            var menu = new Atomic.UIMenuWindow(target, target.id + " popup");
            menu.show(src);
            return true;

        }

        return false;

    }


    handlePopupMenu(target: Atomic.UIWidget, refid: string): boolean {

        if (target.id == "menu atomic editor popup") {

            if (refid == "quit")
                Atomic.getEngine().exit();

        }

        if (target.id == "menu edit popup") {

            if (refid == "edit play") {

                new ToolCore.PlayCmd().run();
                return true;

            }

            return false;

        }
    }

    handleResourceEditorChanged(data):void {

        var editor = <Editor.ResourceEditor> data.editor;

        if (editor) {

            this.showInspectorFrame(editor.requiresInspector());

        } else {

            this.showInspectorFrame(false);

        }

    }


}

export = MainFrame;
