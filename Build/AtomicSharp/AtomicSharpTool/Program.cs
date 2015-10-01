using System;
using System.Reflection;
using AtomicEngine;

namespace AtomicSharpTool
{
	class MainClass
	{
		public static void Main (string[] args)
		{
			Assembly assembly = Assembly.LoadFrom("/Users/josh/Dev/atomic/AtomicGameEngineSharp/Build/AtomicSharp/AtomicSharpTest/bin/Debug/AtomicSharpTest.exe");

			Type[] types = assembly.GetTypes ();

			foreach (var type in types) 
			{
				if (type.BaseType.Name == "CSComponent") {

					FieldInfo[] fields = type.GetFields ();

					Console.WriteLine (type.Name);

					object instance = null;

					foreach (var field in fields)
					{
						foreach (var attribute in field.GetCustomAttributes(true)) {

							if (attribute is InspectorAttribute) {

								if (instance == null)
									instance = Activator.CreateInstance (type);

								var attr = attribute as InspectorAttribute;

								string defaultValue = attr.DefaultValue;
								if (defaultValue.Length == 0 && field.GetValue (instance) != null)
									defaultValue = field.GetValue (instance).ToString ();

							
								Console.WriteLine ("Inspector Field: {0}, {1}, {2}", field.Name, 
									field.FieldType.Name, defaultValue);
								
							}
						}
					}
				}
			}
		}
	}
}
