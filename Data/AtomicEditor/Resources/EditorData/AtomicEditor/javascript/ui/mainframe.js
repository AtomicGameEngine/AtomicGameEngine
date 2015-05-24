var UI = Atomic.UI;
var UIWidget = Atomic.UIWidget;
var graphics = Atomic.getGraphics();

var view = new Atomic.UIView();

var mainframe = new UIWidget();

mainframe.setSize(graphics.width, graphics.height);

// Subscribe to graphics subsystems screen mode switching
mainframe.subscribeToEvent(graphics, "ScreenMode", function(data) {

  mainframe.setSize(data.width, data.height);

});

mainframe.subscribeToEvent("WidgetLoaded", function(data) {

  print("Widget Loaded", data.widget == mainframe);

});

mainframe.subscribeToEvent("BeginFrame", function(data) {

  print("Update", data.frameNumber);

});


mainframe.load("AtomicEditor/editor/ui/mainframe.tb.txt");

view.addChild(mainframe);
