var UI = Atomic.UI;
var UIWidget = Atomic.UIWidget;
var graphics = Atomic.getGraphics();

var view = new Atomic.UIView();

var mainframe = new UIWidget();

mainframe.setSize(graphics.width, graphics.height);

mainframe.subscribeToEvent("ScreenMode", function(data) {

  mainframe.setSize(data.width, data.height);

});

mainframe.subscribeToEvent("WidgetLoaded", function(data) {

  print("Widget Loaded", data.widget == mainframe);

});

mainframe.load("AtomicEditor/editor/ui/mainframe.tb.txt");

view.addChild(mainframe);
