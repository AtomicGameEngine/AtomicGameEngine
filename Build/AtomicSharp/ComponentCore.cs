using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Reflection;

namespace AtomicEngine
{
	internal enum CSComponentMethod
	{
		Start,
		DelayedStart,
		Update,
		PostUpdate,
		FixedUpdate,
		PostFixedUpdate
	};

	static internal class ComponentCore
	{
		static Dictionary<String, Type> componentTypes = new Dictionary<String, Type>();

		static Dictionary<uint, GCHandle> liveComponents = new Dictionary<uint, GCHandle>();

		static uint idGen = 1;

		public static IntPtr CurrentCSComponentNativeInstance = default(IntPtr);

		public static void CallComponentMethod(uint componentID, CSComponentMethod method, float value)
		{
			GCHandle handle;

			if (liveComponents.TryGetValue (componentID, out handle)) {

				var component = (CSComponent)handle.Target;

				switch (method) {

				case CSComponentMethod.Update:
					component.Update (value);
					break;
				}
					
			}
			
		}

		public static void RegisterCSComponent(CSComponent component)
		{
			// keep the instance from being GC'd as it may be referenced solely in native code 
			// attached to a node
			GCHandle handle = GCHandle.Alloc (component);

			// register
			liveComponents [idGen] = handle;
			component.SetManagedID (idGen);
			idGen++;
		}

		// native create CSComponent
		public static void CreateCSComponent(string name, IntPtr nativeCSComponent) 
		{
			Type type;

			if (componentTypes.TryGetValue (name, out type)) {

				// create an instance of the component type
				CurrentCSComponentNativeInstance = nativeCSComponent;
				var component = (CSComponent) Activator.CreateInstance(type);
				CurrentCSComponentNativeInstance = IntPtr.Zero;

				RegisterCSComponent (component);


			
			}

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

