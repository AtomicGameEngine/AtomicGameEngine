using System;
namespace AtomicEngine
{

    public partial class Component : Animatable
    {
        /// <summary>
        /// Whether the component has been explicitly destroyed
        /// </summary>
        public bool Destroyed { get; private set; }

        /// <summary>
        /// Explicitly removes component and disposes of it
        /// </summary>       
        public virtual void Destroy()
        {
            if (Destroyed)
                return;

            Destroyed = true;

            Remove();

            Dispose();

        }

        public T GetComponent<T>(bool recursive = false) where T : Component
        {
            return Node.GetComponent<T>(recursive);
        }

    }

}
