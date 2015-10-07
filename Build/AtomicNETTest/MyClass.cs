
using AtomicEngine;
using System;

namespace AtomicNETTest
{

    public class MyComponent : CSComponent
    {
        public bool MyBoolValue = true;

        [Inspector]
        public float MyFloatValue = 42.0f;

        public string MyStringValue = "Hey!";
    }

}
