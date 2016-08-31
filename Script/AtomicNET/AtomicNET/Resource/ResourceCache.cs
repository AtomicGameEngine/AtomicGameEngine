namespace AtomicEngine
{

    public partial class ResourceCache : AObject
    {

        public T GetResource<T>(string path) where T : Resource
        {
            return (T) GetResource(typeof(T).Name, path);
        }

    }

}
