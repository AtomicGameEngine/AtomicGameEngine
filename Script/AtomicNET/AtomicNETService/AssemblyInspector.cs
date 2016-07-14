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

	class AssemblyInspector
	{

		Dictionary<string, InspectorEnum> InspectorEnums = new Dictionary<string, InspectorEnum> ();
		Dictionary<string, InspectorComponent> InspectorComponents = new Dictionary<string, InspectorComponent> ();

		PEReader peFile;
		MetadataReader metaReader;

		public AssemblyInspector ()
		{

		}

		public string DumpToJSON ()
		{
			var dict = new Dictionary<string, object> ();

			var enumList = new List<object> ();
			var componentList = new List<object> ();

			foreach (var entry in InspectorEnums) {
				enumList.Add (entry.Value.GetJSONDict ());
			}

			foreach (var entry in InspectorComponents) {
				componentList.Add (entry.Value.GetJSONDict ());
			}

			dict ["enums"] = enumList;
			dict ["components"] = componentList;

			return MiniJSON.Json.Serialize (dict);

		}

		public void Inspect (String pathToAssembly)
		{

			using (var stream = File.OpenRead (pathToAssembly))
			using (peFile = new PEReader (stream)) {

				metaReader = peFile.GetMetadataReader ();

				ParseEnums ();
				ParseComponents ();
			}

		}

		void ParseComponents ()
		{

			foreach (var handle in metaReader.TypeDefinitions) {

				var typeDef = metaReader.GetTypeDefinition (handle);

				var baseTypeHandle = typeDef.BaseType;

				if (baseTypeHandle.Kind == HandleKind.TypeReference) {

					var typeRef = metaReader.GetTypeReference ((TypeReferenceHandle)baseTypeHandle);

                    var name = metaReader.GetString (typeRef.Name); 

					if (name != "CSComponent")
						continue;

					var inspector = new CSComponentInspector (typeDef, peFile, metaReader);

					var icomponent = inspector.Inspect ();

					if (icomponent != null)
						InspectorComponents [icomponent.Name] = icomponent;
				}
			}
		}


		void ParseEnums ()
		{
			foreach (var handle in metaReader.TypeDefinitions) {

				var typeDef = metaReader.GetTypeDefinition (handle);

				var baseTypeHandle = typeDef.BaseType;

				if (baseTypeHandle.Kind == HandleKind.TypeReference) {
					var typeRef = metaReader.GetTypeReference ((TypeReferenceHandle)baseTypeHandle);

					if (metaReader.GetString (typeRef.Name) == "Enum") {
						ParseEnum (typeDef);
					}
				}
			}
		}

		void ParseEnum (TypeDefinition enumTypeDef)
		{

			// TODO: verify that int32 is the enums storage type for constant read below

			InspectorEnum ienum = new InspectorEnum ();

			ienum.Name = metaReader.GetString (enumTypeDef.Name);

			InspectorEnums [ienum.Name] = ienum;

			var fields = enumTypeDef.GetFields ();

			foreach (var fieldHandle in fields) {

				var inspectorField = new InspectorField ();

				var fieldDef = metaReader.GetFieldDefinition (fieldHandle);

				if ((fieldDef.Attributes & FieldAttributes.HasDefault) != 0) {

					var constantHandle = fieldDef.GetDefaultValue ();
					var constant = metaReader.GetConstant (constantHandle);

					BlobReader constantReader = metaReader.GetBlobReader (constant.Value);

					ienum.Values [metaReader.GetString (fieldDef.Name)] = constantReader.ReadInt32 ();

				}
			}

			return;

		}
	}

	internal class InspectorEnum
	{
		public String Name;
		public Dictionary<string, int> Values = new Dictionary<string, int> ();

		public Dictionary<string, object> GetJSONDict ()
		{
			var dict = new Dictionary<string,object> ();
			dict ["name"] = Name;
			dict ["values"] = Values;
			return dict;
		}
	}

	internal class InspectorComponent
	{
		public String Name;
		public String Namespace;
		public Dictionary<string, InspectorField> Fields = new Dictionary<string, InspectorField> ();

		public Dictionary<string, object> GetJSONDict ()
		{
			var dict = new Dictionary<string,object> ();

			dict ["name"] = Name;
			dict ["namespace"] = Namespace;

			var fieldList = new List<object> ();

			foreach (var entry in Fields) {
				fieldList.Add (entry.Value.GetJSONDict ());
			}

			dict ["fields"] = fieldList;

			return dict;
		}
	}

	internal class InspectorField
	{

		public Dictionary<string, object> GetJSONDict ()
		{

			var dict = new Dictionary<string,object> ();

			dict ["isEnum"] = IsEnum;
			dict ["typeName"] = TypeName;
			dict ["name"] = Name;
			dict ["defaultValue"] = DefaultValue;

			dict ["caPos"] = CustomAttrPositionalArgs;
			dict ["caNamed"] = CustomAttrNamedArgs;

			return dict;

		}

		public bool IsEnum = false;

		public string TypeName;

		// the Name of the InspectorField
		public string Name;
		// The DefaultValue if supplied
		public string DefaultValue;

		// custom attributes, positional and named
		public List<string> CustomAttrPositionalArgs = new List<string> ();
		public Dictionary<string, string> CustomAttrNamedArgs = new Dictionary<string, string> ();
	}

}
