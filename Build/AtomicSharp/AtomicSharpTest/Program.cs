
using AtomicEngine;
using AtomicPlayer;

class MyGame
{				
	public static void Main (string[] args)
	{
		var player = Atomic.GetSubsystem<Player> ();

		player.LoadScene ("Scenes/Scene.scene");		

		Atomic.Run ();

	}
}


