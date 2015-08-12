
import strings = require("ui/EditorStrings");
import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import MenuItemSources = require("./MenuItemSources");

class MainFrameMenu extends Atomic.ScriptObject {

    constructor() {

        super();

        MenuItemSources.createMenuItemSource("menu atomic editor", editorItems);
        MenuItemSources.createMenuItemSource("menu edit", editItems);
        MenuItemSources.createMenuItemSource("menu file", fileItems);
    }

    handlePopupMenu(target: Atomic.UIWidget, refid: string): boolean {

        if (target.id == "menu atomic editor popup") {

            if (refid == "about atomic editor") {

                EditorUI.getModelOps().showAbout();

                return true;
            }

            if (refid == "manage license") {

              EditorUI.getModelOps().showManageLicense();

              return true;

            }

            if (refid == "quit") {

                this.sendEvent(EditorEvents.Quit);
                return true;

            }

        } else if (target.id == "menu edit popup") {

            if (refid == "edit play") {
                EditorUI.getShortcuts().invokePlay();
                return true;
            }

            if (refid == "edit format code") {
                EditorUI.getShortcuts().invokeFormatCode();
                return true;
            }

            return false;

        } else if (target.id == "menu file popup") {

            if (refid == "file new project") {

                if (ToolCore.toolSystem.project) {

                    EditorUI.showModalError("Project Open",
                        "Please close the current project before creating a new one");

                    return true;
                }

                var mo = EditorUI.getModelOps();
                mo.showNewProject();

                return true;

            }
            if (refid == "file open project") {

                if (ToolCore.toolSystem.project) {

                    EditorUI.showModalError("Project Open",
                        "Please close the current project before opening new one");

                    return true;
                }

                var utils = new Editor.FileUtils();
                var path = utils.openProjectFileDialog();
                if (path) {

                    this.sendEvent(EditorEvents.LoadProject, { path: path });

                }


                return true;

            }

            if (refid == "file close project") {

                this.sendEvent(EditorEvents.CloseProject);

                return true;

            }

            if (refid == "file save file") {
                EditorUI.getShortcuts().invokeFileSave();
                return true;
            }

            if (refid == "file close file") {
                EditorUI.getShortcuts().invokeFileClose();
                return true;
            }

            if (refid == "file save all") {
                this.sendEvent(EditorEvents.SaveAllResources);
                return true;
            }


            return false;
        }

    }


}

export = MainFrameMenu;

// initialization
var StringID = strings.StringID;

var editorItems = {
    "About Atomic Editor": "about atomic editor",
    "-1": null,
    "Manage License": "manage license",
    "-2": null,
    "Check for Updates": "check update",
    "-3": null,
    "Quit": "quit"
};

var editItems = {

    "Undo": ["edit undo", StringID.ShortcutUndo],
    "Redo": ["edit redo", StringID.ShortcutRedo],
    "-1": null,
    "Cut": ["edit cut", StringID.ShortcutCut],
    "Copy": ["edit copy", StringID.ShortcutCopy],
    "Paste": ["edit paste", StringID.ShortcutPaste],
    "Select All": ["edit select all", StringID.ShortcutSelectAll],
    "-2": null,
    "Find": ["edit find", StringID.ShortcutFind],
    "Find Next": ["edit find next", StringID.ShortcutFindNext],
    "Find Prev": ["edit find prev", StringID.ShortcutFindPrev],
    "-3": null,
    "Format Code": ["edit format code", StringID.ShortcutBeautify],
    "-4": null,
    "Play": ["edit play", StringID.ShortcutPlay]

};

var fileItems = {

    "New Project": ["file new project"],
    "Open Project": ["file open project"],
    "Save Project": ["file save project"],
    "-1": null,
    "Close Project": ["file close project"],
    "-2": null,
    "Save File": ["file save file"],
    "Save All Files": ["file save all"],
    "Close File": ["file close file"]
}
