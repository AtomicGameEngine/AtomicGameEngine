

export class ScriptWidget extends Atomic.UIWidget {

	constructor() {

		super();
		
		// JS way of binding method
		// this.subscribeToEvent(this, "WidgetEvent", this.handleWidgetEvent.bind(this));
		
		// TypeScript-ey
		this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

	}

	onEventClick(target: Atomic.UIWidget, refid: string): void {

	}


	handleWidgetEvent(data): boolean {

		if (data.type == Atomic.UI.EVENT_TYPE_CLICK) {

			this.onEventClick(data.target, data.refid);

			return true;

		}

		return false;

	}

}
