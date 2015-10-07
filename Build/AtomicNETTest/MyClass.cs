
using AtomicEngine;
using System;

namespace AtomicNETTest
{
    public enum MyEnum
    {
      One,
      Two = 10,
      Three
    }

    public class MyComponent : CSComponent
    {
        [Inspector]
        public bool MyBoolValue = true;

        [Inspector]
        public int MyIntValue = 5;

        [Inspector]
        public Vector3 MyVector3Value = new Vector3(1, 1, 1);

        [Inspector]
        public Quaternion MyQuaternionValue = new Quaternion(1, 1, 1, 1);

        [Inspector("HeyHeyMyMy", Value2 = "one")]
        public float MyFloatValue = 42.0f;

        [Inspector]
        public string MyStringValue = "Hey!";

        [Inspector]
        public MyEnum MyEnumValue = MyEnum.Three;

        [Inspector("Sprites/star.png")]
        public Sprite2D sprite2D;


    }

}
