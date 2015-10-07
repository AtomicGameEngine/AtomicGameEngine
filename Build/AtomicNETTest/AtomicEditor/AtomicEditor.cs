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
	class AssemblyInspector
	{

		public static void InspectAssembly (String pathToAssembly)
		{

			try {
				using (var stream = File.OpenRead (pathToAssembly))
				using (var peFile = new PEReader (stream)) {

					var reader = peFile.GetMetadataReader ();

					foreach (var handle in reader.TypeDefinitions)
          {
              var typeDef = reader.GetTypeDefinition(handle);

							var baseTypeHandle = typeDef.BaseType;

							if (baseTypeHandle.Kind == HandleKind.TypeReference)
							{
									var typeRef = reader.GetTypeReference((TypeReferenceHandle)baseTypeHandle);

									// TODO: validate assembly of CSComponent typeref
									if (reader.GetString(typeRef.Name) != "CSComponent")
										continue;

									var inspector = new CSComponentInspector(typeDef, peFile, reader);

									inspector.Inspect();

							}


					}

              //uint size, packingSize;
              //bool hasLayout = entry.GetTypeLayout(out size, out packingSize);

							/*
							Console.WriteLine(reader.GetString(entry.Name));

							var fields = entry.GetFields();
							foreach (var fieldHandle in fields)
							{
								 // FieldDefinitionHandle
								 var fieldDef = reader.GetFieldDefinition(fieldHandle);
								 Console.WriteLine("Field! {0}", reader.GetString(fieldDef.Name));
							}

							var methods = entry.GetMethods();

							foreach (var methodHandle in methods)
							{
								 // FieldDefinitionHandle
								 var methodDef = reader.GetMethodDefinition(methodHandle);

								 if (reader.GetString(methodDef.Name) == ".ctor")
								 {
									 Console.WriteLine("Dumping ctor");

									 var body = peFile.GetMethodBody(methodDef.RelativeVirtualAddress);

									 /*
									 var value = CSComponentInspector.Instance.DumpMethod(
									 		reader,
									 		entry,
									 		body.MaxStack,
									 		body.GetILContent(),
									 		ImmutableArray.Create<CSComponentInspector.LocalInfo>(),     // TODO
									 		ImmutableArray.Create<CSComponentInspector.HandlerSpan>());

											Console.WriteLine("IL: \n{0}", value);
										*/

									/*
								 }


							}

          }
					*/
				}
			} catch (Exception ex) {
				Console.WriteLine (ex.Message);
			}


		}

	}

}
