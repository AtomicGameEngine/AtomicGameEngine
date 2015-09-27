
using System;
using AtomicEngine;
using AtomicPlayer;

class Spinner : CSComponent
{
	public float Speed = 1.0f;

	override public void Start()
	{
		
	}

	override public void Update(float timeStep)
	{
		Node.Yaw (timeStep * 75 * Speed, TransformSpace.TS_LOCAL);
	}

}
	
class MyGame
{
	public static void Main (string[] args)
	{		

		Atomic.RegisterAssemblyComponents (typeof(MyGame).Assembly);

		var player = Atomic.GetSubsystem<Player> ();

		player.LoadScene ("Scenes/Scene.scene", null);		

		var scene = player.CurrentScene;

		var zone = scene.GetComponent <Zone> (true);

		var name = zone.Node.Name;

		var chestNode = scene.GetChild ("Chest", true);
		var c = chestNode.AddComponent <Spinner> ();
		c.Speed = 10.0f;

		zone.SetAmbientColor( new Color(1, 0, 0) );

		Atomic.Run ();

	}
}


