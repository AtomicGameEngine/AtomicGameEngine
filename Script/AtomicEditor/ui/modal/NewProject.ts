//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorUI = require("../EditorUI");
import ModalWindow = require("./ModalWindow");

class NewProject extends ModalWindow {

    constructor() {

        super();

        this.init("Project Type", "AtomicEditor/editor/ui/newproject.tb.txt");

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "cancel") {
                this.hide();
                return true;
            }

            var projectType = "";

            if (id == "project_empty") {
                projectType = "EmptyProject/";
            }
            else if (id == "project_2d") {
                projectType = "Project2D/";
            }
            else if (id == "project_3d") {
                projectType = "Project3D/";
            }

            if (projectType) {

                var env = ToolCore.toolEnvironment;
                var projectTemplateFolder = env.toolDataDir + "ProjectTemplates/" + projectType;

                this.hide();

                var ops = EditorUI.getModelOps();
                ops.showCreateProject(projectTemplateFolder);


            }

        }
    }
}


export = NewProject;
