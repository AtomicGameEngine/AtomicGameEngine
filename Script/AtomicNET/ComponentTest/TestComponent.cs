using System;
using AtomicEngine;

namespace ComponentTest
{

    public enum BehaviorState
    {
        Friendly,
        Aggressive,
        Neutral
    }

    public class TestComponent : CSComponent
    {
        [Inspector]
        public bool MyBoolValue = true;

        [Inspector]
        public int MyIntValue = 5;

        [Inspector]
        public int MyOtherIntValue = 101;

        [Inspector]
        public Vector3 MyVector3Value = new Vector3(1, 1, 1);

        [Inspector]
        public Quaternion MyQuaternionValue = new Quaternion(1, 0, 0, 0);

        [Inspector()]
        public float MyFloatValue = 42.0f;

        [Inspector]
        public string MyStringValue = "Hey!";

        [Inspector]
        public BehaviorState State = BehaviorState.Neutral;

        [Inspector("Textures/chest.png")]
        public Sprite2D MySprite2DValue;

        [Inspector(DefaultValue = "Textures/chest.png")]
        public Sprite2D MyOtherSprite2DValue;
    }

}
