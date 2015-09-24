using System;

using AtomicSharp;

namespace AtomicSharpTest
{
	class MainClass
	{
		public static void Main (string[] args)
		{
			AtomicSharp.AtomicSharp.Initialize ();

			while (AtomicSharp.AtomicSharp.RunFrame ()) {
			}
		}
	}
}
