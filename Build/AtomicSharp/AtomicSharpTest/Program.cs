using System;

using AtomicSharp;
using Light = AtomicSharp.Light;
using Node = AtomicSharp.Node;

namespace AtomicSharpTest
{
	class MainClass
	{
		static Node CreateNode() {

			return new AtomicSharp.Node ();
			
		}

		static void AddLight(Node node) {

			node.AddComponent (new Light (), 0, CreateMode.REPLICATED);

		}

		public static void Main (string[] args)
		{
			AtomicSharp.AtomicSharp.Initialize ();

			var node = CreateNode ();
			AddLight (node);

			node.Name = "MyNode";

			if (node.HasComponent("Light"))
				Console.Write (node.Name + " has a light");

			while (AtomicSharp.AtomicSharp.RunFrame ()) {
				
			}
		}
	}
}
