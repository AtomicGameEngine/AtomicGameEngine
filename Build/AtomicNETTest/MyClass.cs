
using AtomicEngine;
using System;

namespace AtomicNETTest
{
    public enum BehaviorState
    {
      Friendly,
      Aggressive,
      Neutral
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
        public Quaternion MyQuaternionValue = new Quaternion(1, 0, 0, 0);

        [Inspector(Value1 = "This is a", Value2 = "test")]
        public float MyFloatValue = 42.0f;

        [Inspector]
        public string MyStringValue = "Hey!";

        [Inspector]
        public BehaviorState State = BehaviorState.Neutral;

        [Inspector("Sprites/star.png")]
        public Sprite2D sprite2D;

    }

}
