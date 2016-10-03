using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using static System.Reflection.IntrospectionExtensions;

namespace AtomicEngine
{

    public partial class Node : Animatable
    {

        public void RemoveComponent<T>() where T : Component
        {
            var type = typeof(T);

            var name = type.Name;

            // TODO: This doesn't allow removing of a specific component type derived from CSComponent
            if (type.GetTypeInfo().IsSubclassOf(typeof(CSComponent)))
            {
                name = "CSComponent";

                throw new InvalidOperationException("Node.RemoveComponent - Add functionality to remove specific CSComponent derived instances");
            }

            RemoveComponent(name);

        }


        public T CreateComponent<T>(CreateMode mode = CreateMode.REPLICATED, uint id = 0) where T : Component
        {
            var type = typeof(T);

            if (type.GetTypeInfo().IsSubclassOf(typeof(CSComponent)))
            {
                Component component = (Component)Activator.CreateInstance(type);
                AddComponent(component, id, mode);
                return (T)component;
            }

            return (T)CreateComponent(type.Name, mode, id);
        }

        public void AddComponent(Component component)
        {
            var type = component.GetType();
            AddComponent(component, 0, CreateMode.REPLICATED);
        }

        public void GetChildrenWithComponent<T>(Vector<Node> dest, bool recursive = false)
        {
            var type = typeof(T);

            // If we're a CSComponent, get "CSComponents" native side and filter here
            if (type.GetTypeInfo().IsSubclassOf(typeof(CSComponent)))
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

        public T GetComponent<T>(bool recursive = false) where T : Component
        {
            return (T)GetComponent(typeof(T).Name, recursive);
        }

        public void GetComponents<T>(Vector<T> dest, bool recursive = false) where T : Component
        {
            Vector<Component> components = new Vector<Component>();
            GetComponents(components, typeof(T).Name, recursive);
            for (int i = 0; i < components.Size; i++)
            {
                dest.Push((T)components[i]);
            }
        }

        public void GetDerivedComponents<T>(Vector<T> dest, bool recursive = false) where T : Component
        {
            Vector<Component> components = new Vector<Component>();
            GetComponents(components, recursive);
            for (int i = 0; i < components.Size; ++i)
            {
                T t = components[i] as T;
                if (t != null)
                    dest.Push(t);
            }
        }

        public T GetCSComponent<T>(bool recursive = false) where T : CSComponent
        {
            Vector<Component> components = new Vector<Component>();
            GetComponents(components, nameof(CSComponent), recursive);
            for (int i = 0; i < components.Size; i++)
            {
                if (components[i].GetType() == typeof(T))
                    return (T) components[i];
            }

            return null;
        }

        public void GetCSComponents<T>(Vector<T> dest, bool recursive = false) where T : CSComponent
        {
            Vector<Component> components = new Vector<Component>();
            GetComponents(components, nameof(CSComponent), recursive);
            for (int i = 0; i < components.Size; i++)
            {
                Component component = components[i];
                if (component.GetType() == typeof(T))
                    dest.Push((T)component);
            }
        }

        public void GetDerivedCSComponents<T>(Vector<T> dest, bool recursive = false) where T : CSComponent
        {
            Vector<Component> components = new Vector<Component>();
            GetComponents(components, nameof(CSComponent), recursive);
            for (int i = 0; i < components.Size; ++i)
            {
                T t = components[i] as T;
                if (t != null)
                    dest.Push(t);
            }
        }
    }

}
