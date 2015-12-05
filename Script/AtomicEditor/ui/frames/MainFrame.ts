//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import ProjectFrame = require("./ProjectFrame");
import ResourceFrame = require("./ResourceFrame");
import WelcomeFrame = require("./WelcomeFrame");
import InspectorFrame = require("./inspector/InspectorFrame");
import HierarchyFrame = require("./HierarchyFrame");
import MainToolbar = require("ui//MainToolbar");

import UIEvents = require("ui/UIEvents");

import ScriptWidget = require("ui/ScriptWidget");
import MainFrameMenu = require("./menus/MainFrameMenu");

import MenuItemSources = require("./menus/MenuItemSources");

class MainFrame extends ScriptWidget {

    constructor() {

        super();

        this.load("AtomicEditor/editor/ui/mainframe.tb.txt");

        this.inspectorlayout = <Atomic.UILayout> this.getWidget("inspectorlayout");

        this.getWidget("consolecontainer").visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;

        this.inspectorframe = new InspectorFrame();
        this.inspectorlayout.addChild(this.inspectorframe);

        this.projectframe = new ProjectFrame(this);
        this.hierarchyFrame = new HierarchyFrame(this);

        this.welcomeFrame = new WelcomeFrame(this);
        this.resourceframe = new ResourceFrame(this);

        this.mainToolbar = new MainToolbar(this.getWidget("maintoolbarcontainer"));

        this.menu = new MainFrameMenu();

        this.disableProjectMenus();

        this.subscribeToEvent(UIEvents.ResourceEditorChanged, (data) => this.handleResourceEditorChanged(data));

        this.subscribeToEvent("ProjectLoaded", (data) => {
            this.showWelcomeFrame(false);
            this.enableProjectMenus();
        });

        this.subscribeToEvent("ProjectUnloaded", (data) => {
            this.showWelcomeFrame(true);
            this.disableProjectMenus();
        });

        this.showWelcomeFrame(true);

    }

    frameVisible(frame: Atomic.UIWidget): boolean {

        var container = <Atomic.UILayout> this.getWidget("resourceviewcontainer");

        var child = null;
        for (child = container.firstChild; child; child = child.next) {
            if (child == frame)
                return true;
        }

        return false;
    }

    showWelcomeFrame(show: boolean) {

        if (show) {
            this.showInspectorFrame(false);
            this.welcomeFrame.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;
            this.resourceframe.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
        }
        else {
            this.showInspectorFrame(true);
            this.resourceframe.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;
            this.welcomeFrame.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
        }

    }

    showInspectorFrame(show: boolean) {

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

        var src = MenuItemSources.getMenuItemSource(target.id);

        if (src) {

            var menu = new Atomic.UIMenuWindow(target, target.id + " popup");
            menu.show(src);
            return true;

        }

        return false;

    }

    disableProjectMenus() {
        this.getWidget("menu edit").setStateRaw(Atomic.UI_WIDGET_STATE_DISABLED);
        this.getWidget("menu build").setStateRaw(Atomic.UI_WIDGET_STATE_DISABLED);
        this.getWidget("menu tools").setStateRaw(Atomic.UI_WIDGET_STATE_DISABLED);
    }

    enableProjectMenus() {
        this.getWidget("menu edit").setStateRaw(Atomic.UI_WIDGET_STATE_NONE);
        this.getWidget("menu build").setStateRaw(Atomic.UI_WIDGET_STATE_NONE);
        this.getWidget("menu tools").setStateRaw(Atomic.UI_WIDGET_STATE_NONE);
    }

    shutdown() {

        this.resourceframe.shutdown();
        this.deleteAllChildren();

    }

    handleResourceEditorChanged(data): void {

        var editor = <Editor.ResourceEditor> data.editor;

        if (editor) {

            //this.showInspectorFrame(editor.requiresInspector());

        } else {

            //this.showInspectorFrame(false);

        }

    }

    projectframe: ProjectFrame;
    resourceframe: ResourceFrame;
    inspectorframe: InspectorFrame;
    hierarchyFrame: HierarchyFrame;
    welcomeFrame: WelcomeFrame;
    inspectorlayout: Atomic.UILayout;
    mainToolbar: MainToolbar;
    menu: MainFrameMenu;

}

export = MainFrame;
