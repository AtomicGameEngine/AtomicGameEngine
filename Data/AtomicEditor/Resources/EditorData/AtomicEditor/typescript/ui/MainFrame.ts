
import strings = require("./EditorStrings");
import menubar = require("./MainFrameMenu");
import scriptwidget = require("./ScriptWidget");

export class MainFrame extends scriptwidget.ScriptWidget {

	constructor(view: Atomic.UIView, width: number, height: number) {

		super();

		this.load("AtomicEditor/editor/ui/mainframe.tb.txt");

		this.setSize(width, height);

	}

	onEventClick(target: Atomic.UIWidget, refid: string): void {

		if (this.handlePopupMenu(target, refid))
			return;

		var src = menubar.getMenuItemSource(target.id);

		if (src) {

			var menu = new Atomic.UIMenuWindow(target, target.id + " popup");
			menu.show(src);

		}

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

	// override example	
	setSize(width: number, height: number): void {

		super.setSize(width, height);

	}
}
