//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import ScriptWidget = require("ui/ScriptWidget");
import Preferences = require("editor/Preferences");

class WelcomeFrame extends ScriptWidget {

    constructor(parent: Atomic.UIWidget) {

        super();

        this.load("AtomicEditor/editor/ui/welcomeframe.tb.txt");

        var recentProjects = <Atomic.UILayout>this.getWidget("recentprojects");
        this.gravity = Atomic.UI_GRAVITY_ALL;

        this.recentList = new Atomic.UIListView();
        this.recentList.rootList.id = "recentList";

        recentProjects.addChild(this.recentList);

        var container = <Atomic.UILayout>parent.getWidget("resourceviewcontainer");

        container.addChild(this);

        this.updateRecentProjects();

        this.subscribeToEvent(EditorEvents.CloseProject, () => {
            this.updateRecentProjects();
        });

        this.initExampleBrowser();

    }

    handleClickedExample(example: ExampleFormat) {

      var ops = EditorUI.getModelOps();
      var env = ToolCore.toolEnvironment;
      ops.showCreateProject(env.toolDataDir + "AtomicExamples/" + example.folder + "/", this.exampleInfoDir + example.screenshot);

    }

    addExample(example: ExampleFormat) {

        var exlayout = <Atomic.UILayout>this.getWidget("examples_layout");

        if (!this.currentExampleLayout) {
            this.currentExampleLayout = new Atomic.UILayout();
            this.currentExampleLayout.spacing = 8;
            exlayout.addChild(this.currentExampleLayout);
        }

        // 200x150

        var exampleLayout = new Atomic.UILayout();
        exampleLayout.skinBg = "StarCondition";
        exampleLayout.axis = Atomic.UI_AXIS_Y;
        exampleLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        exampleLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

        // IMAGE BUTTON

        var id = example.name;

        var button = new Atomic.UIButton();
        button.skinBg = "StarButton";
        button.id = id;
        var image = new Atomic.UIImageWidget();

        button.onClick = () => {

          this.handleClickedExample(example);

        }

        image.image = this.exampleInfoDir + example.screenshot;
        image.skinBg = "ImageFrame";
        var rect = [0, 0, image.imageWidth / 2, image.imageHeight / 2];
        image.rect = rect;

        // NAME FIELD
        var nameField = new Atomic.UITextField();
        nameField.skinBg = "ImageCaption";
        nameField.text = example.name;

        var nameRect = [0, image.imageHeight / 2 - 16, image.imageWidth / 2, image.imageHeight / 2];

        nameField.rect = nameRect;

        nameField.gravity = Atomic.UI_GRAVITY_BOTTOM;

        image.addChild(nameField);

        button.addChild(image);

        var lp = new Atomic.UILayoutParams();
        lp.minWidth = image.imageWidth / 2;
        lp.minHeight = image.imageHeight / 2;

        button.layoutParams = lp;

        button.gravity = Atomic.UI_GRAVITY_LEFT;

        exampleLayout.addChild(button);

        // DESC TEXT

        var descField = new Atomic.UIEditField();
        descField.styling = true;
        descField.multiline = true;
        descField.readOnly = true;
        descField.wrapping = true;

        var styleDesc = "<color #A9A9A9>" + example.desc + "</color>";

        descField.text = styleDesc;

        descField.adaptToContentSize = true;

        lp.height = 42;

        lp.width = image.imageWidth / 2;

        descField.layoutParams = lp;

        exampleLayout.addChild(descField);

        this.currentExampleLayout.addChild(exampleLayout);

        this.exampleCount++;
        // three across, todo, be smarter about this
        if (!(this.exampleCount % 3)) {
            this.currentExampleLayout = null;
        }

    }

    initExampleBrowser() {

        var env = ToolCore.toolEnvironment;

        this.exampleInfoDir =env.toolDataDir + "ExampleInfo/";

        var exampleJsonFile = this.exampleInfoDir + "Examples.json";

        var jsonFile = new Atomic.File(exampleJsonFile, Atomic.FILE_READ);
        if (!jsonFile.isOpen())
            return;

        var examples = <ExamplesFormat>JSON.parse(jsonFile.readText());

        for (var i in examples.examples) {

            this.addExample(examples.examples[i]);
        }

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_RIGHT_POINTER_UP) {
            if (ev.target.id == "recentList") {
                this.openFrameMenu(ev.x, ev.y);
            }
        }

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "open project") {

                var utils = new Editor.FileUtils();
                var path = utils.openProjectFileDialog();
                if (path) {

                    this.sendEvent(EditorEvents.LoadProject, { path: path });

                }

                return true;
            }

            if (id == "new project") {

                var mo = EditorUI.getModelOps();
                mo.showNewProject();
                return true;

            }

            if (id == "recentList") {
                var path: string = this.recent[this.recentList.getSelectedItemID()];
                this.sendEvent(EditorEvents.LoadProject, { path: path });
            }

            if (id == "recentProjectsContextMenu") {
                var prefs = Preferences.getInstance();
                if (ev.refid == "clear recent projects") {
                    prefs.deleteRecentProjects();
                    this.updateRecentProjects();
                }
            }
        }
    }

    updateRecentProjects() {

        this.recentList.deleteAllItems();

        var prefs = Preferences.getInstance();
        prefs.updateRecentProjects();

        this.recent = prefs.recentProjects;

        for (var i in this.recent) {
            this.recentList.addRootItem(this.recent[i], "Folder.icon", i);
        }

    }

    private openFrameMenu(x: number, y: number) {
        var menu = new Atomic.UIMenuWindow(this, "recentProjectsContextMenu");
        var menuButtons = new Atomic.UISelectItemSource();
        menuButtons.addItem(new Atomic.UISelectItem("Clear Recent Projects", "clear recent projects"));
        menu.show(menuButtons, x, y);
    }

    // examples
    exampleInfoDir: string;
    exampleCount = 0;
    currentExampleLayout: Atomic.UILayout;
    exampleInfos:[ExampleFormat];

    recent: string[] = [];
    recentList: Atomic.UIListView;

}

class ExamplesFormat {

    examples: [ExampleFormat];

}

class ExampleFormat {
    name: string;
    desc: string;
    screenshot: string;
    folder: string;
    module: string;
}

export = WelcomeFrame;
