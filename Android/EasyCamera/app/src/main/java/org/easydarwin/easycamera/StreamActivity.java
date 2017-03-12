/*
	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easycamera;

import android.app.ActivityManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

import org.easydarwin.adapter.SpninnerArrayAdapter;
import org.easydarwin.bus.PushOK;
import org.easydarwin.push.EasyPusher;
import org.easydarwin.push.MediaStream;
import org.easydarwin.updatemgr.UpdateMgr;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

@SuppressWarnings("deprecation")
public class StreamActivity extends AppCompatActivity implements SurfaceHolder.Callback, View.OnClickListener {

    static final String TAG = "EasyPusher";

    //默认分辨率
    int width = 640, height = 480;
    SurfaceView surfaceView;
    Button btnSwitch;
    Button btnSetting;
    TextView txtStreamAddress;
    Button btnSwitchCemera;
    Spinner spnResolution;
    List<String> listResolution;
    List<String> listResolutionName;

    MediaStream mMediaStream;
    TextView txtStatus;
    private BroadcastReceiver mReceiver;
    private StatusInfoView mDbgInfoPrint;
    public static boolean mIsActive;
    EasyPusher.OnInitPusherCallback mPusherCallBack;
    private JSONObject mStartStreamingReqBody;
    private int mStartStreamingReqSeq;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        spnResolution = (Spinner) findViewById(R.id.spn_resolution);

        txtStatus = (TextView) findViewById(R.id.txt_stream_status);
        btnSwitch = (Button) findViewById(R.id.btn_switch);
        btnSwitch.setOnClickListener(this);
        btnSetting = (Button) findViewById(R.id.btn_setting);
        btnSetting.setOnClickListener(this);
        btnSwitchCemera = (Button) findViewById(R.id.btn_switchCamera);
        btnSwitchCemera.setOnClickListener(this);
        txtStreamAddress = (TextView) findViewById(R.id.txt_stream_address);

        mDbgInfoPrint = (StatusInfoView) findViewById(R.id.dbg_status_info);
        initDbgInfoView();

        mIsActive = true;

        surfaceView = (SurfaceView) findViewById(R.id.sv_surfaceview);
        surfaceView.getHolder().addCallback(this);
        surfaceView.getHolder().setFixedSize(getResources().getDisplayMetrics().widthPixels,
                getResources().getDisplayMetrics().heightPixels);
        surfaceView.setOnClickListener(this);
        surfaceView.setKeepScreenOn(true);

        listResolution = new ArrayList<String>(Arrays.asList("1280x720", "640x480", "320x240"));
        listResolutionName = new ArrayList<String>(Arrays.asList("高清", "标清", "流畅"));
//        listResolution = new ArrayList<String>();
//        listResolution = Util.getSupportResolution(this);
//        boolean supportdefault = listResolution.contains(String.format("%dx%d", width, height));
//        if (!supportdefault) {
//            String r = listResolution.get(0);
//            String[] splitR = r.split("x");
//            width = Integer.parseInt(splitR[0]);
//            height = Integer.parseInt(splitR[1]);
//        }

        mMediaStream = new MediaStream(getApplicationContext(), surfaceView.getHolder());
        mMediaStream.updateResolution(width, height);
        mMediaStream.setDgree(getDgree());
        initSpninner();

        mPusherCallBack = new EasyPusher.OnInitPusherCallback() {

            @Override
            public void onCallback(int code) {
                switch (code) {
                    case CODE.EASY_ACTIVATE_INVALID_KEY:
                        sendMessage("无效Key");
                        break;
                    case CODE.EASY_ACTIVATE_SUCCESS:
                        sendMessage("激活成功");
                        break;
                    case CODE.EASY_PUSH_STATE_CONNECTING:
                        sendMessage("连接中");
                        break;
                    case CODE.EASY_PUSH_STATE_CONNECTED:
                        sendMessage("连接成功");
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                if (mStartStreamingReqBody != null) {
                                    EasyApplication.sMainBus.post(new PushOK(mStartStreamingReqBody, mStartStreamingReqSeq));
                                    mStartStreamingReqBody = null;
                                }
                            }
                        });
                        break;
                    case CODE.EASY_PUSH_STATE_CONNECT_FAILED:
                        sendMessage("连接失败");
                        break;
                    case CODE.EASY_PUSH_STATE_CONNECT_ABORT:
                        sendMessage("连接异常中断");
                        break;
                    case CODE.EASY_PUSH_STATE_PUSHING:
//                        sendMessage("推流中");

                        break;
                    case CODE.EASY_PUSH_STATE_DISCONNECTED:
                        sendMessage("断开连接");
                        break;
                    case CODE.EASY_ACTIVATE_PLATFORM_ERR:
                        sendMessage("平台不匹配");
                        break;
                    case CODE.EASY_ACTIVATE_COMPANY_ID_LEN_ERR:
                        sendMessage("断授权使用商不匹配");
                        break;
                    case CODE.EASY_ACTIVATE_PROCESS_NAME_LEN_ERR:
                        sendMessage("进程名称长度不匹配");
                        break;
                }
            }
        };

        mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                if (CommandService.ACTION_START_STREAM.equals(intent.getAction())) {
                    final String ip = intent.getStringExtra("Server_IP");
                    final int port = intent.getIntExtra("Server_PORT", 0);
                    final String serial = intent.getStringExtra("Serial");
                    final String channel = intent.getStringExtra("Channel");

                    try {
                        mStartStreamingReqSeq = intent.getIntExtra("seq", 0);
                        mStartStreamingReqBody = new JSONObject(intent.getStringExtra("body"));
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }

                    if (!mMediaStream.isStreaming()) {
                        mMediaStream.startStream(ip, String.valueOf(port), serial, mPusherCallBack);
                        String info = String.format("EasyPusher start pushing to rtsp://%s:%s/%s", ip, port, serial);
                        showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);
                    }
                } else if (CommandService.ACTION_STOP_STREAM.equals(intent.getAction())) {
                    mMediaStream.stopStream();
//                    btnSwitch.setText("开始");
//                    txtStreamAddress.setText(null);
                } else if (CommandService.ACTION_COMMOND_RESTART_THREAD.equals(intent.getAction())) {
                    stopService(new Intent(StreamActivity.this, CommandService.class));
                    if (mIsActive) {
                        try {
                            Thread.sleep(3000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }

                    if (mIsActive) {
                        startService(new Intent(StreamActivity.this, CommandService.class));
                    }
                }
            }
        };

        UpdateMgr update = new UpdateMgr(this);
        update.checkUpdate();

        final IntentFilter inf = new IntentFilter(CommandService.ACTION_START_STREAM);
        inf.addAction(CommandService.ACTION_STOP_STREAM);
        inf.addAction(CommandService.ACTION_COMMOND_RESTART_THREAD);
        LocalBroadcastManager.getInstance(this).registerReceiver(mReceiver, inf);
        startService(new Intent(this, CommandService.class));
    }

    private void initDbgInfoView() {
        if (mDbgInfoPrint == null)
            return;
        ViewGroup.LayoutParams lp = mDbgInfoPrint.getLayoutParams();
        lp.height = getResources().getDisplayMetrics().heightPixels / 3;
        mDbgInfoPrint.setLayoutParams(lp);
        mDbgInfoPrint.requestLayout();
        mDbgInfoPrint.setInstence(mDbgInfoPrint);
    }

    private static final String STATE = "state";
    private static final int MSG_STATE = 1;

    private void sendMessage(String message) {
        Message msg = Message.obtain();
        msg.what = MSG_STATE;
        Bundle bundle = new Bundle();
        bundle.putString(STATE, message);
        msg.setData(bundle);
        handler.sendMessage(msg);
    }

    Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_STATE:
                    String state = msg.getData().getString("state");
                    txtStatus.setText(state);
                    break;
            }
        }
    };

    //    private void initSpninner() {
//        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, R.layout.spn_item, listResolution);
//        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
//        spnResolution.setAdapter(adapter);
//        int position = listResolution.indexOf(String.format("%dx%d", width, height));
//        spnResolution.setSelection(position);
//        spnResolution.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
//            @Override
//            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
//                String r = listResolution.get(position);
//                String[] splitR = r.split("x");
//                width = Integer.parseInt(splitR[0]);
//                height = Integer.parseInt(splitR[1]);
//                mMediaStream.updateResolution(width, height);
//                mMediaStream.reStartStream();
//            }
//
//            @Override
//            public void onNothingSelected(AdapterView<?> parent) {
//
//            }
//        });
//    }
    private void initSpninner() {
        SpninnerArrayAdapter adapter = new SpninnerArrayAdapter(this, R.layout.spn_item, listResolutionName);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spnResolution.setAdapter(adapter);
        spnResolution.setSelection(1, false);
        spnResolution.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                String r = listResolution.get(position);
                String[] splitR = r.split("x");
                width = Integer.parseInt(splitR[0]);
                height = Integer.parseInt(splitR[1]);
                mMediaStream.updateResolution(width, height);
                mMediaStream.reStartStream();
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        mMediaStream.createCamera();
        mMediaStream.startPreview();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        mMediaStream.stopPreview();
        mMediaStream.stopStream();
        mMediaStream.destroyCamera();
    }


    private int getDgree() {
        int rotation = getWindowManager().getDefaultDisplay().getRotation();
        int degrees = 0;
        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 0;
                break; // Natural orientation
            case Surface.ROTATION_90:
                degrees = 90;
                break; // Landscape left
            case Surface.ROTATION_180:
                degrees = 180;
                break;// Upside down
            case Surface.ROTATION_270:
                degrees = 270;
                break;// Landscape right
        }
        return degrees;
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_switch:
//
                break;
            case R.id.btn_setting:
                startActivity(new Intent(this, SettingActivity.class));
                break;
            case R.id.sv_surfaceview:
                try {
                    mMediaStream.getCamera().autoFocus(null);
                } catch (Exception e) {
                }
                break;
            case R.id.btn_switchCamera: {
                mMediaStream.setDgree(getDgree());
                mMediaStream.switchCamera();
            }
            break;
        }
    }

    @Override
    protected void onDestroy() {
        StatusInfoView.getInstence().uninit();
        super.onDestroy();
        stopService(new Intent(this, CommandService.class));
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mReceiver);
        handler.removeCallbacksAndMessages(null);
        mMediaStream.destroyStream();
    }

    @Override
    protected void onStop() {
        super.onStop();
        if (isBackground(this)) {
            //app 进入后台
            mIsActive = false;
            mMediaStream.stopStream();
            stopService(new Intent(StreamActivity.this, CommandService.class));
        }
        Log.v(TAG, "kim onStop mIsActive=" + mIsActive);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (!mIsActive) {
            mIsActive = true;
            startService(new Intent(StreamActivity.this, CommandService.class));
        }
        Log.v(TAG, "kim onResume mIsActive=" + mIsActive);
    }

    public static boolean isBackground(Context context) {
        ActivityManager activityManager = (ActivityManager) context
                .getSystemService(Context.ACTIVITY_SERVICE);

        List<ActivityManager.RunningAppProcessInfo> appProcesses = activityManager
                .getRunningAppProcesses();
        for (ActivityManager.RunningAppProcessInfo appProcess : appProcesses) {
            if (appProcess.processName.equals(context.getPackageName())) {
                /*
                BACKGROUND=400 EMPTY=500 FOREGROUND=100
                GONE=1000 PERCEPTIBLE=130 SERVICE=300 ISIBLE=200
                 */
                Log.i(context.getPackageName(), "此appimportace ="
                        + appProcess.importance
                        + ",context.getClass().getName()="
                        + context.getClass().getName());
                if (appProcess.importance != ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND) {
                    Log.i(context.getPackageName(), "处于后台" + appProcess.processName);
                    return true;
                } else {
                    Log.i(context.getPackageName(), "处于前台" + appProcess.processName);
                    return false;
                }
            }
        }
        return false;
    }

    private void showDbgMsg(String level, String data) {
        Intent intent = new Intent(StatusInfoView.DBG_MSG);
        intent.putExtra(StatusInfoView.DBG_LEVEL, level);
        intent.putExtra(StatusInfoView.DBG_DATA, data);
        LocalBroadcastManager.getInstance(StreamActivity.this).sendBroadcast(intent);
    }
}
