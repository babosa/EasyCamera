package org.easydarwin.push;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.media.MediaCodec;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;
import java.util.Iterator;
import java.util.List;

public class CameraManager {
    private static final String TAG = "CameraManager";
    private Activity mContext;
    private Camera mCamera;
    private Handler mHandler;
    private boolean mPreviewing;
    private int mCreateRefCount = 0;
    private int mStartRefCount = 0;

    public static boolean determineClosestSupportedPictureResolution(Camera.Parameters parameters, int[] size) {
        int minDist = Integer.MAX_VALUE;
        int width = 0, height = 0;
        String supportedSizesStr = "Supported resolutions: ";
        List<Camera.Size> supportedSizes = parameters.getSupportedPictureSizes();
        for (Iterator<Camera.Size> it = supportedSizes.iterator(); it.hasNext(); ) {
            Camera.Size sz = it.next();
            supportedSizesStr += sz.width + "x" + sz.height + (it.hasNext() ? ", " : "");
            int dist = Math.abs(size[0] - sz.width);
            if (dist < minDist) {
                minDist = dist;
                width = sz.width;
                height = sz.height;
            }
        }
        Log.v(TAG, supportedSizesStr);
        if (size[0] != width || size[1] != height) {
            Log.v(TAG, "Resolution modified: " + size[0] + "x" + size[1] + "->" + width + "x" + height);
            size[0] = width;
            size[1] = height;
            return true;
        }
        return false;
    }

    public static void setCameraDisplayOrientation(Activity activity,
                                                   int cameraId, Camera camera) {
        Camera.CameraInfo info =
                new Camera.CameraInfo();
        Camera.getCameraInfo(cameraId, info);
        int rotation = activity.getWindowManager().getDefaultDisplay()
                .getRotation();
        int degrees = 0;
        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 0;
                break;
            case Surface.ROTATION_90:
                degrees = 90;
                break;
            case Surface.ROTATION_180:
                degrees = 180;
                break;
            case Surface.ROTATION_270:
                degrees = 270;
                break;
        }

        int result;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degrees) % 360;
            result = (360 - result) % 360;  // compensate the mirror
        } else {  // back-facing
            result = (info.orientation - degrees + 360) % 360;
        }
        camera.setDisplayOrientation(result);
    }

    public CameraManager() {
        new HandlerThread("CAMERA") {
            @Override
            protected void onLooperPrepared() {
                super.onLooperPrepared();
                mHandler = new Handler() {
                    @Override
                    public void handleMessage(Message msg) {
                        super.handleMessage(msg);
                        Log.e(TAG, String.format("msg what %s comming", msg.what));
                        switch (msg.what) {
                            case 0:
                                mPreviewing = false;
                                int cameraId = msg.arg1;
                                mCamera = Camera.open(cameraId);
                                setCameraDisplayOrientation(mContext, 0, mCamera);
                                break;
                            case 1:
                                if (mCamera != null) {
                                    try {
                                        SurfaceTexture texture = (SurfaceTexture) msg.obj;
                                        mCamera.setPreviewTexture(texture);
                                        final Camera.Parameters parameters = mCamera.getParameters();
                                        int[] size = new int[]{msg.arg1, msg.arg2};
                                        determineClosestSupportedPictureResolution(parameters, size);
                                        parameters.setPictureSize(size[0], size[1]);
//            parameters.setRecordingHint(true);
                                        mCamera.setParameters(parameters);
                                        mCamera.startPreview();
                                        mPreviewing = true;
                                    } catch (IOException ex) {
                                        ex.printStackTrace();
                                    }
                                }
                                break;
                            case 2:
                                if (mCamera != null && mPreviewing) {
                                    Camera.PictureCallback callback = (Camera.PictureCallback) msg.obj;
                                    mCamera.takePicture(null, null, null, callback);
                                }
                                break;
                            case -1:
                                if (mCamera != null) {
                                    mCamera.stopPreview();
                                    mPreviewing = false;
                                }
                                break;
                            case -2:
                                if (mCamera != null) {
                                    mCamera.release();
                                    mCamera = null;
                                }
                                break;
                            case -3:
                                quit();
                                mHandler = null;
                        }
                    }
                };
                synchronized (CameraManager.this) {
                    CameraManager.this.notify();
                }
            }
        }.start();
        synchronized (this) {
            try {
                wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public void createCamera(Activity context, int cameraId) {
        mContext = context;
        Log.e(TAG, String.format("create Camera, createRefCount:%d", mCreateRefCount));
        if (mCreateRefCount++ >= 1) {   // 如果已经创建过了。就不再创建
            Log.e(TAG, "已经创建过了。就不再创建");
            return;
        }
        if (mHandler != null) {
            mHandler.obtainMessage(0, cameraId, 0);
        }

    }

    public void startPreview(int width, int height, SurfaceTexture texture, Runnable previewCallback) {
        Log.e(TAG, String.format("start Camera, startRefCount:%d", mStartRefCount));
        if (mStartRefCount++ >= 1) {   // 如果已经启动过了。就不再启动
            Log.e(TAG, "已经启动过了。就不再启动");
            return;
        }
        if (mHandler != null) {
            mHandler.obtainMessage(1, width, height, texture).sendToTarget();
            mHandler.post(previewCallback);
        }
    }

    public void stopPreview() {
        Log.e(TAG, String.format("stop Camera, startRefCount:%d", mStartRefCount));
        if (mStartRefCount-- < 1) {   // 如果已经关闭过了。就不再关闭
            Log.e(TAG, "已经关闭过了。就不再关闭");
            return;
        }
        if (mHandler != null) {
            mHandler.sendEmptyMessage(-1);
        }
    }

    public void releaseCamera() {
        Log.e(TAG, String.format("release Camera, createRefCount:%d", mCreateRefCount));
        if (mCreateRefCount-- < 1) {   // 如果已经释放过了。就不再释放
            Log.e(TAG, "已经释放过了。就不再释放");
            return;
        }
        if (mHandler != null) {
            mHandler.sendEmptyMessage(-2);
        }
        mContext = null;
    }

    public void takePicture(Camera.PictureCallback callback) {
        mHandler.obtainMessage(2, callback).sendToTarget();
    }
    MediaCodec mMediaCodec;
    public void stopMediaCodec() {

        if (mMediaCodec != null) {
            mMediaCodec.stop();
            mMediaCodec.release();
            mMediaCodec = null;
        }
    }
}
