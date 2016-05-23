using System;
using AtomicEngine;

namespace ComponentTest
{
    public class Spinner : CSComponent
    {
        [Inspector()]
        public float Speed = 1.0f;

        void Update(float timeStep)
        {
            Node.Yaw(timeStep * Speed * 70);
        }

    }

}
