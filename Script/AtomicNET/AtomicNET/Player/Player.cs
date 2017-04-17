
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using AtomicEngine;

namespace AtomicPlayer
{
    public partial class Player : AObject
    {
        static List<Scene> loadedScenes = new List<Scene>();

        internal override void PostNativeUpdate()
        {
            SubscribeToEvent<PlayerSceneLoadBeginEvent>(e =>
            {
                loadedScenes.Add(e.Scene);
            });

            SubscribeToEvent<PlayerSceneLoadEndEvent>(e =>
            {
                if (!e.Success)
                    loadedScenes.Remove(e.Scene);
            });

            SubscribeToEvent<PlayerSceneUnloadEvent>(e =>
            {
                if (loadedScenes.Contains(e.Scene))
                    loadedScenes.Remove(e.Scene);

                e.Scene.Destroy();
            });

        }


    }

}

