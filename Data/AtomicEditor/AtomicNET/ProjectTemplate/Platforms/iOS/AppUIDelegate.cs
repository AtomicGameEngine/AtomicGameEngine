using Foundation;
using UIKit;
using System.Threading.Tasks;

using AtomicEngine;
$$APPLICATION_NAMESPACE$$

namespace AtomicPlayer
{
    [Register("AppUIDelegate")]
    public partial class AppUIDelegate : UIApplicationDelegate
    {
        public override bool FinishedLaunching(UIApplication app, NSDictionary options)
        {
            LaunchGame();
            return true;
        }

        async void LaunchGame()
        {
            await Task.Yield();
            Application.Run<$$APPLICATION_APPDELEGATECLASS$$>(new string[0]);
        }
    }
}
