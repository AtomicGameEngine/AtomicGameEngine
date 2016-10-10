using System;
namespace AtomicEngine
{

    public partial class Component : Animatable
    {

        public T GetComponent<T>(bool recursive = false) where T : Component
        {
            return Node.GetComponent<T>(recursive);
        }

    }

}
