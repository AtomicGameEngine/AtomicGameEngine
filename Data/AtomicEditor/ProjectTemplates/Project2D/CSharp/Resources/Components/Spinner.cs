using System;
using AtomicEngine;

public class Spinner : CSComponent
{

    //inspector fields to make speed variable visible in editor
    [Inspector]
    float speed = 1.0f;    

    //update function is called each frame
    //timeStep: time since last call to update in seconds
    void Update(float timeStep)
    {
        Node.Roll(speed * timeStep * 75.0f);
    }

}
