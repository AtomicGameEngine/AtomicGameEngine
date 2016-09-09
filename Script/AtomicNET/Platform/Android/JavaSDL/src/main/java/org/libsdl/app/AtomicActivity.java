package org.libsdl.app;

import android.app.Activity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.widget.AbsoluteLayout;

/**
 * Created by Egor on 13/10/15.
 */
public class AtomicActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        SDLSurface surface = SDLActivity.createSurface(this);

        AbsoluteLayout mLayout = new AbsoluteLayout(this);
        mLayout.addView(surface);
        setContentView(mLayout);
    }

    @Override
    protected void onResume() {
        super.onResume();
        SDLActivity.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        SDLActivity.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        SDLActivity.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        SDLActivity.onLowMemory();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        SDLActivity.onWindowFocusChanged(hasFocus);
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        if (!SDLActivity.dispatchKeyEvent(event))
            return false;

        return super.dispatchKeyEvent(event);
    }
}
