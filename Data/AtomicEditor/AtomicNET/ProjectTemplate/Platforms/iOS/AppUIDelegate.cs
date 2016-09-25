using Foundation;
using UIKit;
using System.Threading.Tasks;

using AtomicEngine;

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
			Application.Run<AtomicMain>(new string[0]);
		}
	}
}
