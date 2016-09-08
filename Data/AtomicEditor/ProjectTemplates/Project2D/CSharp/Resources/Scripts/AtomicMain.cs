using AtomicEngine;
using AtomicPlayer;

public class AtomicMain : AppDelegate
{
    public override void Start()
    {
        AtomicNET.GetSubsystem<Player>().LoadScene("Scenes/Scene.scene");
    }

}
