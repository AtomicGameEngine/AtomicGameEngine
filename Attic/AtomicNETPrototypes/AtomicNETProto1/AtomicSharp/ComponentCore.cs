using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Reflection;

namespace AtomicEngine
{
	static internal class ComponentCore
	{
		static Dictionary<String, Type> componentTypes = new Dictionary<String, Type>();

		// holds a reference
		static Dictionary<uint, CSComponent> liveComponents = new Dictionary<uint, CSComponent>();

		public static IntPtr CurrentCSComponentNativeInstance = default(IntPtr);

		public static void CallComponentMethod(uint componentID, CSComponentMethod method, float value)
		{
			CSComponent component;

			if (liveComponents.TryGetValue (componentID, out component)) {

				switch (method) {

				//case CSComponentMethod.Start:
				//	component.Start ();
				//	break;

				//case CSComponentMethod.Update:
				//	component.Update (value);
				//	break;
				}

			}

		}

		public static void RegisterCSComponent(CSComponent component)
		{
			// register (holds a reference)
			liveComponents [component.RefID] = component;
		}

		public static void DestroyComponent(Component component)
		{
			var c = component as CSComponent;

			if (c != null) {

				liveComponents.Remove (c.RefID);
			}
		}

		// native create CSComponent
		public static IntPtr CreateCSComponent(string name)
		{
			Type type;

			if (componentTypes.TryGetValue (name, out type)) {

				// create an instance of the component type
				var component = (CSComponent) Activator.CreateInstance(type);

				return component.nativeInstance;

			}

			return IntPtr.Zero;

		}

		static void RegisterComponentType(Type componentType)
		{
			// TODO: Check for name clash, we don't want to use assembly qualified names, etc to keep it simple
			componentTypes [componentType.Name] = componentType;
		}

		public static void RegisterAssemblyComponents(Assembly assembly)
		{
			Type csComponentType = typeof(CSComponent);
			Type[] types = assembly.GetTypes ();

			List<Type> componentTypes = new List<Type> ();

			foreach (Type type in types)
			{
				for (var current = type.BaseType; current != null; current = current.BaseType) {

					if (current == csComponentType) {
						componentTypes.Add(type);
						break;
					}
				}
			}

			foreach (Type type in componentTypes) {

				RegisterComponentType (type);

			}

		}

	}
}
