namespace AtomicEngine
{

    public partial class ResourceCache : AObject
    {

        public T GetResource<T>(string path) where T : Resource
        {
            return (T) GetResource(typeof(T).Name, path);
        }

        public T Get<T>(string path) where T : Resource
        {
            return (T)GetResource(typeof(T).Name, path);
        }

    }

}
