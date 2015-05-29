import menubar = require("./MainFrameMenu");
import scriptwidget = require("./ScriptWidget");
import projectframe = require("./ProjectFrame");
import MessageModal = require("./modal/MessageModal");
import UIEvents = require("./UIEvents");

export class MainFrame extends scriptwidget.ScriptWidget {

	projectframe:projectframe.ProjectFrame;
	private messagemodal:MessageModal.MessageModal = new MessageModal.MessageModal();

	constructor() {

		super();

		this.load("AtomicEditor/editor/ui/mainframe.tb.txt");

		this.projectframe = new projectframe.ProjectFrame(this);

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

}
