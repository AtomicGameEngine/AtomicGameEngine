
import MainFrame = require("./MainFrame");

export class MyUI {
	
	constructor() {

		var graphics = Atomic.getGraphics();

		this.view = new Atomic.UIView();

		this.mainframe = new MainFrame.MainFrame(this.view, graphics.width, graphics.height);

		this.view.addChild(this.mainframe);

	}

	view: Atomic.UIView;

	mainframe: MainFrame.MainFrame;


}

export var ui = new MyUI();


