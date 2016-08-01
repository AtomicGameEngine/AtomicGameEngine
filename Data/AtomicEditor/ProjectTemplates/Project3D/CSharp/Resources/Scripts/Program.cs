using AtomicEngine;
using AtomicPlayer;

public static class AtomicMain
{
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    static void Main(string[] args)
    {

        AtomicNET.GetSubsystem<Player>().LoadScene("Scenes/Scene.scene");

    }
    
}
