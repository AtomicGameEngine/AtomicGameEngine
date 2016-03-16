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
