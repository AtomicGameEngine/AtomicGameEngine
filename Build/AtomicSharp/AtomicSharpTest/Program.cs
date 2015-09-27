
using System;
using AtomicEngine;
using AtomicPlayer;

class Spinner : CSComponent
{
	public float Speed = 1.0f;

	override public void Start()
	{
		//var renderer = Atomic.GetSubsystem<Renderer> ();
		//SubscribeToEvent (renderer, "BeginViewUpdate", handleEvent);

		SubscribeToEvent ("BeginViewUpdate", handleEvent);
	}

	void handleEvent(VariantMap eventData)
	{
		View view = eventData.Get<View> ("view");
		view.Camera.Zoom = zoom;
		zoom += .01f;
	}

	override public void Update(float timeStep)
	{
		Node.Yaw (timeStep * 75 * Speed, TransformSpace.TS_LOCAL);
	}

	float zoom = 1.0f;

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

		var chestNode = scene.GetChild ("Chest", true);
		var c = chestNode.AddComponent <Spinner> ();
		c.Speed = 10.0f;

		c.Destroy ();

		zone.SetAmbientColor( new Color(1, 0, 0) );

		Atomic.Run ();

	}
}


