
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


    public class Spinner : CSComponent
    {

        [Inspector]
        float speed = 1.0f;

        [Inspector]
        float pitchSpeed = 0.0f;

        public override void Update(float timeStep)
        {
          Node.Yaw(timeStep * speed * 75.0f);
          Node.Pitch(timeStep * pitchSpeed * 75.0f);

          //Console.WriteLine("TICK! : {0}", speed);
        }
    }

    public class MyComponent : CSComponent
    {

        public override void Update(float timeStep)
        {
            //Node.Yaw(timeStep * 75);

        }

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
