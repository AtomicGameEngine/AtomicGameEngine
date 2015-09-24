
using AtomicEngine;
using AtomicPlayer;

class MyGame
{				
	public static void Main (string[] args)
	{
		var player = Atomic.GetSubsystem<Player> ();

		player.LoadScene ("Scenes/Scene.scene");		

		var scene = player.CurrentScene;

		var zone = scene.GetComponent <Zone> (true);

		zone.SetAmbientColor( new Color(1, 0, 0) );

		Atomic.Run ();

	}
}


