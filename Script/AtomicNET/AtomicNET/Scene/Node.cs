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


    }

}
