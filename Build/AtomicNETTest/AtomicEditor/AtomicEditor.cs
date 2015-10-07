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

namespace AtomicEditor
{
	class AtomicEditor
	{

		public static void InspectAssembly (String pathToAssembly)
		{

			try {

				var inspector = new AssemblyInspector();
				inspector.Inspect(pathToAssembly);
				Console.WriteLine(inspector.DumpToJSON());

			} catch (Exception ex) {
				Console.WriteLine (ex.Message);
			}


		}

	}

}
