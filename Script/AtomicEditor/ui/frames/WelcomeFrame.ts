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

import EditorUI = require("ui/EditorUI");
import ScriptWidget = require("ui/ScriptWidget");
import Preferences = require("editor/Preferences");
import ProjectTemplates = require("resources/ProjectTemplates");

class WelcomeFrame extends ScriptWidget {

    constructor(parent: Atomic.UIWidget) {

        super();

        this.load("AtomicEditor/editor/ui/welcomeframe.tb.txt");

        var recentProjects = <Atomic.UILayout>this.getWidget("recentprojects");

        this.examplesLayout = <Atomic.UILayout>this.getWidget("examples_layout");
        this.examplesCSharp = <Atomic.UIButton>this.getWidget("examples_csharp");
        this.examplesJavaScript = <Atomic.UIButton>this.getWidget("examples_javascript");
        this.examplesTypeScript = <Atomic.UIButton>this.getWidget("examples_typescript");

        this.examplesCSharp.onClick = () => { this.handleExampleFilter(); };
        this.examplesJavaScript.onClick = () => { this.handleExampleFilter(); };
        this.examplesTypeScript.onClick = () => { this.handleExampleFilter(); };

        this.gravity = Atomic.UI_GRAVITY.UI_GRAVITY_ALL;

        this.recentList = new Atomic.UIListView();
        this.recentList.rootList.id = "recentList";

        recentProjects.addChild(this.recentList);

        var container = <Atomic.UILayout>parent.getWidget("resourceviewcontainer");

        container.addChild(this);

        this.updateRecentProjects();

        this.subscribeToEvent(Editor.EditorCloseProjectEvent(() => {
            this.updateRecentProjects();
        }));

        this.initExampleBrowser();

    }

    handleClickedExample(example: ProjectTemplates.ProjectTemplateDefinition) {

        var ops = EditorUI.getModelOps();
        ops.showCreateProject(example);
    }

    addExample(example: ProjectTemplates.ProjectTemplateDefinition) {

        var fileSystem = Atomic.getFileSystem();

        let languages = [];
        if (this.examplesCSharp.value) {
            languages.push("CSharp");
        }
        if (this.examplesJavaScript.value) {
            languages.push("JavaScript");
        }
        if (this.examplesTypeScript.value) {
            languages.push("TypeScript");
        }

        // If user doesn't select any languages, show 'em all'
        if (!languages.length) {
            languages = ["CSharp", "JavaScript", "TypeScript"];
        }

        let exists = false;

        for (var i = 0; i < languages.length; i++) {

            if (example.languages.indexOf(languages[i]) != -1) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            return;
        }

        if (!this.currentExampleLayout) {
            this.currentExampleLayout = new Atomic.UILayout();
            this.currentExampleLayout.spacing = 8;
            this.examplesLayout.addChild(this.currentExampleLayout);
        }

        // 200x150

        var exampleLayout = new Atomic.UILayout();
        exampleLayout.skinBg = "StarCondition";
        exampleLayout.axis = Atomic.UI_AXIS.UI_AXIS_Y;
        exampleLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        exampleLayout.layoutSize = Atomic.UI_LAYOUT_SIZE.UI_LAYOUT_SIZE_AVAILABLE;

        // IMAGE BUTTON

        var id = example.name;

        var button = new Atomic.UIButton();
        button.skinBg = "StarButton";
        button.id = id;
        var image = new Atomic.UIImageWidget();

        button.onClick = () => {

            this.handleClickedExample(example);

        };

        image.image = example.screenshot;
        image.skinBg = "ImageFrame";
        var rect = [0, 0, image.imageWidth / 2, image.imageHeight / 2];
        image.rect = rect;

        // NAME FIELD
        var nameField = new Atomic.UITextField();
        nameField.skinBg = "ImageCaption";
        nameField.text = example.name;

        var nameRect = [0, image.imageHeight / 2 - 16, image.imageWidth / 2, image.imageHeight / 2];

        nameField.rect = nameRect;

        nameField.gravity = Atomic.UI_GRAVITY.UI_GRAVITY_BOTTOM;

        image.addChild(nameField);

        button.addChild(image);

        var lp = new Atomic.UILayoutParams();
        lp.minWidth = image.imageWidth / 2;
        lp.minHeight = image.imageHeight / 2;

        button.layoutParams = lp;

        button.gravity = Atomic.UI_GRAVITY.UI_GRAVITY_LEFT;

        exampleLayout.addChild(button);

        // DESC TEXT

        var descField = new Atomic.UIEditField();
        descField.styling = true;
        descField.multiline = true;
        descField.readOnly = true;
        descField.wrapping = true;

        descField.skinBg = "AccentColor4";

        descField.text = example.desc;

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

    handleExampleFilter() {

        this.initExampleBrowser();

    }

    initExampleBrowser() {

        this.exampleCount = 0;
        this.examplesLayout.deleteAllChildren();
        this.currentExampleLayout = null;

        let examples = ProjectTemplates.getExampleProjectTemplateDefinitions();
        for (var i = 0; i < examples.length; i++) {
            this.addExample(examples[i]);
        }
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_RIGHT_POINTER_UP) {
            if (ev.target.id == "recentList") {
                this.openFrameMenu(ev.x, ev.y);
            }
        }

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "open project") {

                var utils = new Editor.FileUtils();
                var path = utils.openProjectFileDialog();
                if (path) {

                    this.sendEvent(Editor.RequestProjectLoadEventData({ path: path }));

                }

                return true;
            }

            if (id == "new project") {

                var mo = EditorUI.getModelOps();
                mo.showNewProject();
                return true;

            }

            if (id == "recentList") {
                if (!this.recentList.getSelectedItemID()) {
                    return;
                }
                var path: string = this.recent[this.recentList.getSelectedItemID()];
                this.sendEvent(Editor.RequestProjectLoadEventData({ path: path }));
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
    exampleCount = 0;

    currentExampleLayout: Atomic.UILayout;
    examplesLayout:Atomic.UILayout;

    examplesCSharp: Atomic.UIButton;
    examplesJavaScript: Atomic.UIButton;
    examplesTypeScript: Atomic.UIButton;

    recent: string[] = [];
    recentList: Atomic.UIListView;

}

export = WelcomeFrame;
