using Android.App;
using Android.Content.PM;
using Android.Widget;
using Android.OS;
using Android.Views;
using AtomicEngine;

$$APPLICATION_NAMESPACE$$

namespace AtomicPlayer
{
    [Activity(Label = "AtomicPlayer", MainLauncher = true,
        Icon = "@drawable/icon", Theme = "@android:style/Theme.NoTitleBar.Fullscreen",
        ConfigurationChanges = ConfigChanges.KeyboardHidden | ConfigChanges.Orientation,
        ScreenOrientation = ScreenOrientation.Landscape)]
    public class MainActivity : Activity
    {
        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);
            var mLayout = new AbsoluteLayout(this);
            var surface = AndroidSDLSurface.CreateSurface(this, false, typeof($$APPLICATION_APPDELEGATECLASS$$));
            mLayout.AddView(surface);
            SetContentView(mLayout);
        }

        protected override void OnResume()
        {
            AndroidSDLSurface.OnResume();
            base.OnResume();
        }

        protected override void OnPause()
        {
            AndroidSDLSurface.OnPause();
            base.OnPause();
        }

        public override void OnLowMemory()
        {
            AndroidSDLSurface.OnLowMemory();
            base.OnLowMemory();
        }

        protected override void OnDestroy()
        {
            AndroidSDLSurface.OnDestroy();
            base.OnDestroy();
        }

        public override bool DispatchKeyEvent(KeyEvent e)
        {
            if (!AndroidSDLSurface.DispatchKeyEvent(e))
                return false;
            return base.DispatchKeyEvent(e);
        }

        public override void OnWindowFocusChanged(bool hasFocus)
        {
            AndroidSDLSurface.OnWindowFocusChanged(hasFocus);
            base.OnWindowFocusChanged(hasFocus);
        }
    }
}
