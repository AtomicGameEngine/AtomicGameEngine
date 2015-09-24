using System;
using AtomicEngine;
using AtomicPlayer;

namespace AtomicSharpTest
{
	class MainClass
	{				
		public static void Main (string[] args)
		{
			Atomic.Initialize ();

			var player = Atomic.GetSubsystem<Player> ();

			player.LoadScene ("Scenes/Scene.scene", null);		

			while (Atomic.RunFrame ()) {
						
				
			}
		}
	}
}
