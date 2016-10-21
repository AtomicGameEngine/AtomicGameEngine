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

        public System.IO.Stream GetFileStream(string name, bool sendEventOnFailure = true)
        {
            File file = GetFile(name, sendEventOnFailure);
            if (file != null &&
                file.IsOpen())
            {
                return file.ToStream();
            }

            return null;
        }
    }
}
