using System;

using Atomic;
using AtomicPlayer;

namespace AtomicSharpTest
{
	class MainClass
	{
		static Scene CreateScene() {

			var node = new Node ();
			var player = (Player)node.GetSubsystem ("Player");
			player.LoadScene ("Scenes/Scene.scene", null);		
			return player.CurrentScene;

		}
				
		public static void Main (string[] args)
		{
			Atomic.AtomicSharp.Initialize ();

			CreateScene ();

			while (Atomic.AtomicSharp.RunFrame ()) {
						
				
			}
		}
	}
}
