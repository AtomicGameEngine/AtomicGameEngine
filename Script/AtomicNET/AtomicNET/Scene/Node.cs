using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public partial class Node : Animatable
    {

        public T CreateComponent<T>(CreateMode mode = CreateMode.REPLICATED, uint id = 0) where T : Component
        {
            var type = typeof(T);

            if (type.IsSubclassOf(typeof(CSComponent)))
            {
                Component component = (Component)Activator.CreateInstance(type);
                CSComponentCore.RegisterInstance((CSComponent) component);
                AddComponent(component, id, mode);
                return (T) component;
            }

            return (T) CreateComponent(type.Name, mode, id);
        }

		public void GetChildrenWithComponent<T>(Vector<Node> dest, bool recursive = false)
		{
			var type = typeof(T);

			// If we're a CSComponent, get "CSComponents" native side and filter here
			if (type.IsSubclassOf(typeof(CSComponent)))
			{
				Vector<Node> temp = new Vector<Node>();

				GetChildrenWithComponent(temp, "CSComponent", recursive);

				// filter based on actual type

				for (uint i = 0; i < temp.Size; i++)
				{
					var node = temp[i];

					Vector<Component> components = new Vector<Component>();

					node.GetComponents(components, "Component", false);

					for (uint j = 0; j < components.Size; j++)
					{
						if (components[j].GetType() == type)
						{
							dest.Push(node);
							break;
						}						
					}
				}

				return;
			}

			GetChildrenWithComponent(dest, type.Name, recursive);
		}

    }

}
