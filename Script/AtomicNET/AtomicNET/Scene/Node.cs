using System;
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
            dest.Clear();
            GetComponents(ComponentVector, typeof(T).Name, recursive);
            for (int i = 0; i < ComponentVector.Size; i++)
            {
                if (ComponentVector[i] != null)
                    dest.Push((T)ComponentVector[i]);
            }
            ComponentVector.Clear();
        }

        public void GetDerivedComponents<T>(Vector<T> dest, bool recursive = false) where T : Component
        {
            dest.Clear();
            GetComponents(ComponentVector, typeof(Component).Name, recursive);
            for (int i = 0; i < ComponentVector.Size; ++i)
            {
                T t = ComponentVector[i] as T;
                if (t != null)
                    dest.Push(t);
            }
            ComponentVector.Clear();
        }

        public T GetCSComponent<T>(bool recursive = false) where T : CSComponent
        {
            GetComponents(ComponentVector, nameof(CSComponent), recursive);
            for (int i = 0; i < ComponentVector.Size; i++)
            {
                Component component = ComponentVector[i];
                if (component != null &&
                    component.GetType() == typeof(T))
                {
                    ComponentVector.Clear();
                    return (T)component;
                }
            }

            ComponentVector.Clear();
            return null;
        }

        public void GetCSComponents<T>(Vector<T> dest, bool recursive = false) where T : CSComponent
        {
            dest.Clear();
            GetComponents(ComponentVector, nameof(CSComponent), recursive);
            for (int i = 0; i < ComponentVector.Size; i++)
            {
                Component component = ComponentVector[i];
                if (component != null &&
                    component.GetType() == typeof(T))
                    dest.Push((T)component);
            }
            ComponentVector.Clear();
        }

        public void GetDerivedCSComponents<T>(Vector<T> dest, bool recursive = false) where T : CSComponent
        {
            dest.Clear();
            GetComponents(ComponentVector, nameof(CSComponent), recursive);
            for (int i = 0; i < ComponentVector.Size; ++i)
            {
                T t = ComponentVector[i] as T;
                if (t != null)
                    dest.Push(t);
            }
            ComponentVector.Clear();
        }

        // Reuse vectors to avoid churn, but don't have one on every referenced node
        // Wrapping in a static property, instead of just immediate static allocation,
        // Because Runtime isn't ready at time of static initialization: 
        // https://github.com/AtomicGameEngine/AtomicGameEngine/issues/1512
        private static Vector<Component> ComponentVector => lazyComponentVector.Value;
        private static Lazy<Vector<Component>> lazyComponentVector = new Lazy<Vector<Component>>();
    }

}
