
using System;
using AtomicEngine;
using AtomicPlayer;

class MyObject : ScriptObject
{
	
}

class Spinner : CSComponent
{
	public float Speed = 1.0f;

	override public void Start()
	{
		myObject = new MyObject ();

		SubscribeToEvent (myObject, "MyEvent", handleMyObjectEvent);

		var renderer = Atomic.GetSubsystem<Renderer> ();
		SubscribeToEvent (renderer, "BeginViewUpdate", handleEvent);

		SubscribeToEvent (this, "SelfEvent", handleSelfEvent);

		SendEvent ("SelfEvent");
	}

	void handleSelfEvent(VariantMap eventData)
	{
		Console.WriteLine ("Got Self Event");		
	}
		
	void handleMyObjectEvent(VariantMap eventData)
	{
		//Console.WriteLine ("Got My Event");		
	}

	void handleEvent(VariantMap eventData)
	{
		View view = eventData.Get<View> ("view");
		view.Camera.Zoom = zoom;
		zoom += .01f;

		myObject.SendEvent ("MyEvent");
	}

	override public void Update(float timeStep)
	{
		Node.Yaw (timeStep * 75 * Speed);
	}

	float zoom = 1.0f;

	MyObject myObject;

}
	
class MyGame
{
	public static void Main (string[] args)
	{		

		Atomic.RegisterAssemblyComponents (typeof(MyGame).Assembly);

		var player = Atomic.GetSubsystem<Player> ();
		var graphics = Atomic.GetSubsystem<Graphics> ();

		Console.WriteLine ("{0}, {1}", graphics.Width, graphics.Height);

		player.LoadScene ("Scenes/Scene.scene", null);		

		var scene = player.CurrentScene;

		var zone = scene.GetComponent <Zone> (true);

		var name = zone.Node.Name;

		/*
		var chestNode = scene.GetChild ("Chest", true);
		var c = chestNode.AddComponent <Spinner> ();
		c.Speed = 10.0f;
		c.Destroy ();
		*/

		zone.SetAmbientColor( new Color(1, 0, 0) );

		Atomic.Run ();

	}
}


