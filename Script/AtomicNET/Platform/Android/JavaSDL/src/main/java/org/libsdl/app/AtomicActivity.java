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

    // ATOMIC BEGIN
    /**
     * This method is called by SDL if SDL did not handle a message itself.
     * This happens if a received message contains an unsupported command.
     * Method can be overwritten to handle Messages in a different class.
     * @param command the command of the message.
     * @param param the parameter of the message. May be null.
     * @return if the message was handled in overridden method.
     */
    protected boolean onUnhandledMessage(int command, Object param) {
        return false;
    }
    //ATOMIC END

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
