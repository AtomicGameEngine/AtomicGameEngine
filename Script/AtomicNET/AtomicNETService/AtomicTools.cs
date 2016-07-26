using System;
using System.IO;
using System.Diagnostics;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.PortableExecutable;
using System.Reflection.Metadata.Ecma335;
using System.Text;

using AtomicEngine;

using File = System.IO.File;

namespace AtomicTools
{
	public class AtomicTools
	{

		public static String InspectAssembly (String pathToAssembly)
		{

			try {

				var inspector = new AssemblyInspector ();
				inspector.Inspect (pathToAssembly);
				return inspector.DumpToJSON();

			} catch (Exception ex) {
				Console.WriteLine (ex.Message);
			}

			return "";

		}

	}

}
