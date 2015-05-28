
import strings = require("./EditorStrings");
import menubar = require("./MainFrameMenu");

export class MainFrame extends Atomic.UIWidget {

	constructor(view: Atomic.UIView, width: number, height: number) {

		super();

		this.load("AtomicEditor/editor/ui/mainframe.tb.txt");

		this.setSize(width, height);

		this.subscribeToEvent(this, "WidgetEvent", function(data) {
			
			// this no longer refers to this MainFrame, 
			// instead it is the this of the function call 
			// look into fixing this

			if (data.handler.handleMenuBarEvent(data)) return true;

			return false;

		});

	}

	handlePopupMenu(data): boolean {

		var target = data.target;

		if (target && target.id == "menu atomic editor popup") {

			if (data.refid == "quit") {

				// todo, send a request for file saves, etc
				Atomic.getEngine().exit();

				return true;

			}

		}

		if (target && target.id == "menu edit popup") {

			if (data.refid == "edit play") {

				new ToolCore.PlayCmd().run();

				return true;

			}

		}


	}

	handleMenuBarEvent(data): boolean {

		if (this.handlePopupMenu(data))
			return true;

		if (data.type == Atomic.UI.EVENT_TYPE_CLICK) {

			//if (mainframe.handleMenuAtomicEditor(data)) return true;

			var target = data.target;

			var src = menubar.getMenuItemSource(target.id);

			if (src) {

				var menu = new Atomic.UIMenuWindow(target, target.id + " popup");
				menu.show(src);
				return true;

			}

		}

		return false;

	}

	setSize(width: number, height: number): void {

		super.setSize(width, height);

	}
}
