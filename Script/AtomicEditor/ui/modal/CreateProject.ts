
import EditorUI = require("../EditorUI");
import ModalWindow = require("./ModalWindow");

class CreateProject extends ModalWindow {

    constructor(templateSourceDir: string, imagePath: string = "") {

        super();

        this.templateSourceDir = templateSourceDir;

        this.init("Create Project", "AtomicEditor/editor/ui/createproject.tb.txt");

        this.projectPathField = <Atomic.UIEditField> this.getWidget("project_path");
        this.projectNameField = <Atomic.UIEditField> this.getWidget("project_name");
        this.image = <Atomic.UIImageWidget> this.getWidget("project_image");

        if (!imagePath)
            this.image.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
        else
            this.image.image = imagePath;


        var fileSystem = Atomic.getFileSystem();

        var userDocuments = fileSystem.userDocumentsDir;

        if (Atomic.platform == "MacOSX") {

            userDocuments += "Documents/AtomicProjects";

        } else {

            userDocuments += "AtomicProjects";

        }

        this.projectPathField.text = userDocuments;

        this.resizeToFitContent();
        this.center();

    }

    checkProjectCreate(): boolean {

        var name = this.projectNameField.text.trim();

        if (!name) {
            EditorUI.showModalError("New Project Editor Error", "Please enter a project name");
            return false;
        }

        var folder = this.projectPathField.text.trim();

        if (!folder)
        {
            EditorUI.showModalError("New Project Editor Error", "Please choose a root project folder");
            return false;
        }

        return true;

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "cancel") {
                this.hide();
                return true;
            }
            else if (id == "choose_path") {
                var utils = new Editor.FileUtils();
                var path = utils.newProjectFileDialog();

                if (path.length)
                    this.projectPathField.text = path;

                return true;
            }
            else if (id == "create") {

                if (this.checkProjectCreate()) {

                }

                return true;

            }



        }
    }

    projectPathField: Atomic.UIEditField;
    projectNameField: Atomic.UIEditField;
    image: Atomic.UIImageWidget;

    templateSourceDir: string;
}


export = CreateProject;
