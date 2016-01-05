//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorUI = require("../EditorUI");
import ModalWindow = require("./ModalWindow");
import ProjectTemplates = require("../../resources/ProjectTemplates");

class NewProject extends ModalWindow {

    constructor() {

        super();

        this.init("Project Type", "AtomicEditor/editor/ui/newproject.tb.txt");

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            let id = ev.target.id;

            if (id == "cancel") {
                this.hide();
                return true;
            }
            let projectDefinition = ProjectTemplates.getNewProjectTemplateDefinition(id);

            if (projectDefinition) {
                this.hide();

                let ops = EditorUI.getModelOps();
                ops.showCreateProject(projectDefinition);
            } else {
                EditorUI.showModalError("New Project Error", `Could not create new project of type: ${id}`);
            }
        }
    }
}


export = NewProject;
