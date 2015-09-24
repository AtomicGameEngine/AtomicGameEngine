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

		static private Node parent;
		static private Node child;

		static void setup()
		{
			parent = CreateNode ();
			AddLight (parent);
			parent.Name = "MyNode";

			child = parent.CreateChild ("Child", CreateMode.REPLICATED, 0);

			child = null;

		}

		public static void Main (string[] args)
		{
			AtomicSharp.AtomicSharp.Initialize ();

			setup ();

			while (AtomicSharp.AtomicSharp.RunFrame ()) {

				child = parent.GetChild ("Child", false);

				if (child.Parent.HasComponent("Light"))
					Console.Write (child.Parent.Name + " has a light\n");

				child = null;
			
				
			}
		}
	}
}
