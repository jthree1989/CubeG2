package com.example.armyc.cubeg2;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

public class MySurfaceView extends GLSurfaceView {
    private MyRenderer mRenderer;
    private float mPreviousX;
    private float mPreviousY;
    private final float TOUCH_SCALE_FACTOR = 180.0f / 320;

    public MySurfaceView(Context context) {
        super(context);
        this.setEGLContextClientVersion(2);
        mRenderer = new MyRenderer(context);
        this.setRenderer(mRenderer);
        this.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    public boolean onTouchEvent(final MotionEvent event) {
        float x = event.getX();
        float y = event.getY();
        switch (event.getAction()) {
            case MotionEvent.ACTION_MOVE:
                float dx = x - mPreviousX;
                float dy = y - mPreviousY;
                mRenderer.mAngleX += dx * TOUCH_SCALE_FACTOR;
                mRenderer.mAngleY += dy * TOUCH_SCALE_FACTOR;
                requestRender();
        }
        mPreviousX = x;
        mPreviousY = y;
        return true;
    }
}
