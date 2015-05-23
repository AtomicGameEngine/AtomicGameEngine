var UI = Atomic.UI;
var UIWidget = Atomic.UIWidget;
var graphics = Atomic.getGraphics();

var view = new Atomic.UIView();

var mainframe = new UIWidget();

mainframe.load("AtomicEditor/editor/ui/mainframe.tb.txt");
mainframe.setSize(graphics.width, graphics.height);

var handleEvent = function() {

  print("Got Event");

}

mainframe.subscribeToEvent("ScreenMode", handleEvent);

view.addChild(mainframe);
