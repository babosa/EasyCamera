package org.easydarwin.easycamera;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.AsyncTask;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Process;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Base64;
import android.util.Log;
import android.util.SparseArray;

import com.squareup.otto.Subscribe;

import org.easydarwin.audio.TalkPlay;
import org.easydarwin.bus.PushOK;
import org.easydarwin.config.Config;
import org.easydarwin.push.MediaStream;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.concurrent.ArrayBlockingQueue;

public class CommandService extends Service implements SharedPreferences.OnSharedPreferenceChangeListener {
    private static final String TAG = "CMD";
    private static final int STATE_CONNECTING = 2;
    private static final String ACTION_COMMOND_STATE_CHANGED = "state-changed";
    private static final String ACTION_COMMOND_KEEPALIVE = "keep-alive";
    public static final String ACTION_COMMOND_RESTART_THREAD = "restart-thread";
    private static final String KEY_STATE = "key-state";
    private static final int STATE_CONNECTED = 3;
    private static final int STATE_CONNECT_ERROR = -1;
    public static final String ACTION_START_STREAM = "action_start_stream";
    public static final String ACTION_STOP_STREAM = "action_stop_stream";
    private Socket mSoc;
    private InputStreamReader mIS;
    private OutputStreamWriter mOS;
    private BroadcastReceiver mReceiver;
    private int mSeq = 100000;
    private SparseArray<String> mResp = new SparseArray<String>();
    private static final int WAIT_RESPONSE_TIMEOUT_VALUE = 20;
    TalkPlay mTalkPlay;
    private static final Handler sHandler = new Handler();

    private static String readLine(InputStreamReader is, int contentLe) throws IOException {
        ArrayList lineByteList = new ArrayList();
        byte readByte;
        int total = 0;
        if (contentLe != 0) {
            do {
                readByte = (byte) is.read();
                lineByteList.add(Byte.valueOf(readByte));
                total++;
            } while (total < contentLe);//消息体读还未读完
        } else {
            do {
                readByte = (byte) is.read();
                lineByteList.add(Byte.valueOf(readByte));
            } while (readByte != 10);
        }

        byte[] tmpByteArr = new byte[lineByteList.size()];
        for (int i = 0; i < lineByteList.size(); i++) {
            tmpByteArr[i] = ((Byte) lineByteList.get(i)).byteValue();
        }
        lineByteList.clear();

        return new String(tmpByteArr, "UTF-8");
    }

    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        if (key.equals(Config.SERVER_IP) || key.equals(Config.SERVER_PORT)) {
            showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, "正在停止服务");
            AsyncTask.execute(new Runnable() {
                @Override
                public void run() {
                    stopService();
                    sHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, "正在启动服务");
                        }
                    });
                    start();
                }
            });
        }
    }

    public class BlockCommandThread extends Thread {
        @Override
        public void run() {
            Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
            Intent i = new Intent(ACTION_COMMOND_STATE_CHANGED);
            i.putExtra(KEY_STATE, STATE_CONNECTING);
            LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(i);
            if (mTalkPlay == null) {
                mTalkPlay = new TalkPlay(getApplicationContext());
            }

            try {
                doRun();
            } catch (Exception e) {
                String info = String.format("client server stop running with exceptions");
                showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_WARN, info);
                e.printStackTrace();
                Intent intent = new Intent(ACTION_COMMOND_STATE_CHANGED);
                intent.putExtra(KEY_STATE, STATE_CONNECT_ERROR);
                LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(intent);
            }
        }

        private String requestAndResp(String requestJson, int seq) throws IOException, InterruptedException {
            String sb = buildOutStreamContent(requestJson);
            mResp.put(seq, null);
            synchronized (mOS) {
                if (mOS == null) throw new IOException("channel not valid.");
                mOS.write(sb);
                mOS.flush();
            }

            synchronized (mResp) {
                int totalMillis = WAIT_RESPONSE_TIMEOUT_VALUE * 1000;
                long timeBegin = System.currentTimeMillis();
                String resp = "";
                while (mReadThread != null && mResp.get(seq) == null) {
                    long waitedMillis = System.currentTimeMillis() - timeBegin;
                    long leftMillis = totalMillis - waitedMillis;
                    if (leftMillis > 0) {
                        mResp.wait(leftMillis);
                    }
                }
                resp = mResp.get(seq);
                mResp.remove(seq);
                return resp;
            }
        }


        private void doRun() throws IOException, JSONException {
            String info = "";
            String ip = EasyApplication.getEasyApplication().getIp();
            String port = EasyApplication.getEasyApplication().getPort();
            String name = EasyApplication.getEasyApplication().getDeviceName();
            String serial = EasyApplication.getEasyApplication().getDeviceSerial();
            String tag = EasyApplication.getEasyApplication().getDeviceTag();
            String token = EasyApplication.getEasyApplication().getDeviceKey();//Token

            InetAddress inetAddress = Inet4Address.getByName(ip);
            byte[] addr = inetAddress.getAddress();
            StringBuffer sb = new StringBuffer();
            for (int j = 0; j < addr.length; j++) {
                short sh = (short) (addr[j] >= 0 ? addr[j] : 256 + addr[j]);
                sb.append(sh);
                if (j != addr.length - 1) {
                    sb.append('.');
                }
            }
            ip = sb.toString();
            int nPort = Integer.parseInt(port);
            Socket s = null;
            int retryCount = 0;
            while (mReadThread != null && s == null) {
                try {
                    Socket tempS = new Socket();
                    tempS.connect(new InetSocketAddress(ip, nPort), 10000);
                    s = tempS;
                } catch (Exception e) {
                    try {
                        retryCount++;
                        Thread.sleep(1000);
                        Log.d(TAG, "kim retryCount=" + retryCount);
                        if (retryCount == 1) {
                            info = String.format("Connect server[%s:%d] error", ip, nPort);
                            showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_WARN, info);
                        }
                    } catch (Exception e1) {
                        //
                    }
                }
            }

            Log.d(TAG, "kim Connect server success");

            info = String.format("Connect server[%s:%d] success", ip, nPort);
            showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);
            mSoc = s;
            s.setSoTimeout(0);
            OutputStreamWriter osw = new OutputStreamWriter(s.getOutputStream());
            mOS = osw;
            String registerJson = String.format("{\n" +
                    "  \"EasyDarwin\": {\n" +
                    "    \"Body\": {\n" +
                    "      \"Name\": \"%s\",\n" +
                    "      \"Serial\": \"%s\",\n" +
                    "      \"Tag\": \"%s\",\n" +
                    "      \"Token\": \"%s\"\n" +
                    "    },\n" +
                    "    \"Header\": {\n" +
                    "      \"AppType\": \"EasyCamera\",\n" +
                    "      \"CSeq\": \"1\",\n" +
                    "      \"MessageType\": \"MSG_DS_REGISTER_REQ\",\n" +
                    "      \"TerminalType\": \"Android\",\n" +
                    "      \"Version\": \"1.0\"\n" +
                    "    }\n" +
                    "  }\n" +
                    "}", name, serial, tag, token);
            String registerContent = buildOutStreamContent(registerJson);

            info = String.format("Send MSG_DS_REGISTER_REQ %s [%s:%s]", serial, ip, port);
            showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);

            osw.write(registerContent);
            osw.flush();

            //--输出服务器传回的消息的头信息
            InputStreamReader is = new InputStreamReader(s.getInputStream());
            mIS = is;
            String line = null;
            int contentLength = 0;//服务器发送回来的消息长度
            // 读取所有服务器发送过来的请求参数头部信息
            do {
                line = readLine(is, 0);
                //如果有Content-Length消息头时取出
                if (line.startsWith("Content-Length")) {
                    contentLength = Integer.parseInt(line.split(":")[1].trim());
                }
                //打印请求部信息
                System.out.print(line);
                if (line.startsWith("HTTP/1.1")) {
                    if (line.equals("HTTP/1.1 200 OK\r\n") == false) {
                        info = String.format("Recv MSG_DS_REGISTER_ACK Fail");
                        showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_WARN, info);
                        Intent i = new Intent(ACTION_COMMOND_STATE_CHANGED);
                        i.putExtra(KEY_STATE, STATE_CONNECT_ERROR);
                        LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(i);
                        return;
                    }
                }
                //如果遇到了一个单独的回车换行，则表示请求头结束
            } while (!line.equals("\r\n"));

            //--输消息的体
            final String str = readLine(is, contentLength);

            if (str.indexOf("\"ErrorNum\" : \"200\"") < 0) {
                info = String.format("Recv MSG_DS_REGISTER_ACK Fail");
                showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_WARN, info);
                Intent i = new Intent(ACTION_COMMOND_STATE_CHANGED);
                i.putExtra(KEY_STATE, STATE_CONNECT_ERROR);
                LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(i);
                return;
            } else {
                mIS = is;
                mOS = osw;
                info = String.format("Recv MSG_DS_REGISTER_ACK");
                showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);

                Intent i = new Intent(ACTION_COMMOND_STATE_CHANGED);
                i.putExtra(KEY_STATE, STATE_CONNECTED);
                LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(i);
                LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(new Intent(MediaStream.ACTION_SNAPSHOT_REQ));
            }

            Log.d(TAG, String.format("kim register result:\n%s", str));

            while (mReadThread != null) {
                do {
                    line = readLine(mIS, 0);
                    //如果有Content-Length消息头时取出
                    if (line.startsWith("Content-Length")) {
                        contentLength = Integer.parseInt(line.split(":")[1].trim());
                    }
                    Log.d(TAG, line);
                } while (!line.equals("\r\n"));

                //--输消息的体
                String resp = readLine(mIS, contentLength);

//                Log.d(TAG, "kim resp = "+resp);

                JSONObject receivedJson = new JSONObject(resp);
                final JSONObject easyDarwin = receivedJson.getJSONObject("EasyDarwin");
                final JSONObject body = easyDarwin.getJSONObject("Body");
                final JSONObject header = easyDarwin.getJSONObject("Header");
                int seq = header.getInt("CSeq");
                if (mResp.indexOfKey(seq) != -1) {
                    mResp.put(seq, resp);
                    synchronized (mResp) {
                        mResp.notifyAll();
                    }
                } else {
                    if ("MSG_SD_PUSH_STREAM_REQ".equals(header.get("MessageType"))) {  // 请求视频
                        final Intent intent = new Intent(ACTION_START_STREAM);
                        intent.putExtra("Server_IP", body.getString("Server_IP"));
                        intent.putExtra("Channel", body.getString("Channel"));
                        intent.putExtra("Server_PORT", body.getInt("Server_PORT"));
                        intent.putExtra("Serial", body.getString("Serial"));
                        intent.putExtra("body", body.toString());
                        intent.putExtra("seq", seq);
                        LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(intent);

                        info = String.format("Recv MSG_SD_PUSH_STREAM_REQ RTSP [%s:%d]", body.getString("Server_IP"), body.getInt("Server_PORT"));
                        showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);
                    } else if ("MSG_SD_STREAM_STOP_REQ".equals(header.get("MessageType"))) {  // 终止视频
                        info = String.format("Recv MSG_SD_STREAM_STOP_REQ");
                        showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);
                        LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(new Intent(ACTION_STOP_STREAM));
                        String respStr = String.format("{\n" +
                                "  \"EasyDarwin\": {\n" +
                                "    \"Body\": {\n" +
                                "      \"Channel\": \"%s\",\n" +
                                "      \"From\" : \"%s\",\n" +
                                "      \"Protocol\": \"%s\",\n" +
                                "      \"Serial\": \"%s\",\n" +
                                "      \"To\" : \"%s\",\n" +
                                "      \"Via\" : \"%s\"\n" +
                                "    },\n" +
                                "    \"Header\": {\n" +
                                "      \"CSeq\": \"%d\",\n" +
                                "      \"ErrorNum\": \"200\",\n" +
                                "      \"ErrorString\": \"Success OK\",\n" +
                                "      \"MessageType\": \"MSG_DS_STREAM_STOP_ACK\",\n" +
                                "      \"Version\": \"1.0\"\n" +
                                "    }\n" +
                                "  }\n" +
                                "}", body.get("Channel"), body.get("To"), body.get("Protocol"), body.get("Serial"), body.get("From"), body.get("Via"), seq);
                        respStr = buildOutStreamContent(respStr);
                        info = String.format("Send MSG_SD_STREAM_STOP_ACK");
                        showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);
                        synchronized (mOS) {
                            mOS.write(respStr);
                            mOS.flush();
                        }
                    } else if ("MSG_SD_CONTROL_TALKBACK_REQ".equals(header.get("MessageType"))) {
                        String cmd = body.getString("Command");
                        if (cmd.equals("START")) {
                            info = String.format("Recv MSG_SD_CONTROL_TALKBACK_REQ START");
                            showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);
                            mTalkPlay.startPlay();
                        } else if (cmd.equals("SENDDATA")) {
                            String pts = body.getString("Pts");
                            String data = body.getString("AudioData");
                            mTalkPlay.inputDataToQueue(pts, data);
                        } else if (cmd.equals("STOP")) {
                            info = String.format("Recv MSG_SD_CONTROL_TALKBACK_REQ STOP");
                            showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);
                            mTalkPlay.stopPlay();
                        }

                        String respStr = String.format("{\n" +
                                "  \"EasyDarwin\": {\n" +
                                "    \"Body\": {\n" +
                                "      \"Channel\": \"%s\",\n" +
                                "      \"From\" : \"%s\",\n" +
                                "      \"Protocol\": \"%s\",\n" +
                                "      \"Reserve\": \"1\",\n" +
                                "      \"Serial\": \"%s\",\n" +
                                "      \"To\" : \"%s\",\n" +
                                "      \"Via\" : \"%s\"\n" +
                                "    },\n" +
                                "    \"Header\": {\n" +
                                "      \"CSeq\": \"%d\",\n" +
                                "      \"ErrorNum\": \"200\",\n" +
                                "      \"ErrorString\": \"Success OK\",\n" +
                                "      \"MessageType\": \"MSG_DS_CONTROL_TALKBACK_ACK\",\n" +
                                "      \"Version\": \"1.0\"\n" +
                                "    }\n" +
                                "  }\n" +
                                "}", body.get("Channel"), body.get("To"), body.get("Protocol"), body.get("Serial"), body.get("From"), body.get("Via"), seq);

                        if (cmd.equals("START") || cmd.equals("STOP")) {
                            info = String.format("Send MSG_DS_CONTROL_TALKBACK_ACK");
                            showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);
                        }

                        respStr = buildOutStreamContent(respStr);
                        synchronized (mOS) {
                            mOS.write(respStr);
                            mOS.flush();
                        }
                    }
                }
            }
        }
    }

    public class KeepAliveThread extends Thread {
        private int count = 0;

        @Override
        public void run() {
            while (mKeepAliveThread != null) {
                String timeStr = EasyApplication.getEasyApplication().getKeepAliveInterval();
                int time = Integer.parseInt(timeStr);
                try {
                    Thread.sleep(time * 1000);
                } catch (Exception e) {
                    e.printStackTrace();
                    Log.d(TAG, "kim retry exception!");
                }

                if (mOS == null || mIS == null)
                    continue;

                if (++count % 3 == 0) {
                    Log.d(TAG, "kim send : " + MediaStream.ACTION_SNAPSHOT_REQ);
                    AsyncTask.execute(new Runnable() {
                        @Override
                        public void run() {
                            doSendSnap();
                        }
                    });
                } else {
                    Log.d(TAG, "kim send : " + ACTION_COMMOND_KEEPALIVE);
                    AsyncTask.execute(new Runnable() {
                        @Override
                        public void run() {
                            doSendKeepalive();
                        }
                    });
                }
            }
        }
    }

    @NonNull
    private String buildOutStreamContent(String requestJson) {
        StringBuffer sb = new StringBuffer();
        sb.append("POST / HTTP/1.1\r\n");
        sb.append("User-Agent:Android device\r\n");
        sb.append("Connection: Keep-Alive\r\n");
        sb.append("Content-Length: ");
        sb.append(requestJson.length());
        sb.append("\r\n");

        //注，这是关键的关键，忘了这里让我搞了半个小时。这里一定要一个回车换行，表示消息头完，不然服务器会等待

        sb.append("\r\n");
        sb.append(requestJson);
        return sb.toString();
    }

    BlockCommandThread mReadThread = null;
    KeepAliveThread mKeepAliveThread = null;


    public CommandService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void doSendSnap() {
        String info = "";
        if (mOS != null) {
            Log.d(TAG, "kim AsyncTask.execute run");
            BlockCommandThread thread = mReadThread;
            if (thread != null) {
                try {
                    final File file = new File(getExternalFilesDir(Environment.DIRECTORY_PICTURES), "snap.jpeg");
                    Log.d(TAG, "kim snap exists=" + file.exists());
                    if (file.exists()) {
                        Log.d(TAG, "kim mSoc.isClosed()=" + mSoc.isClosed() + ", mSoc.isConnected()=" + mSoc.isConnected());


                        FileInputStream fis = new FileInputStream(file);
                        byte[] buffer = new byte[fis.available()];
                        fis.read(buffer);
                        String base64 = Base64.encodeToString(buffer, Base64.NO_PADDING | Base64.NO_WRAP);
                        String serial = EasyApplication.getEasyApplication().getDeviceSerial();

                        info = String.format("Send MSG_DS_POST_SNAP_REQ");
                        showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);

                        String resp = thread.requestAndResp(String.format("{\n" +
                                "  \"EasyDarwin\": {\n" +
                                "    \"Body\": {\n" +
                                "      \"Image\": \"%s\",\n" +
                                "      \"Serial\": \"%s\",\n" +
                                "      \"Time\": \"\",\n" +
                                "      \"Type\": \"JPEG\"\n" +
                                "    },\n" +
                                "    \"Header\": {\n" +
                                "      \"CSeq\": \"%d\",\n" +
                                "      \"MessageType\": \"MSG_DS_POST_SNAP_REQ\",\n" +
                                "      \"Version\": \"1.0\"\n" +
                                "    }\n" +
                                "  }\n" +
                                "}", base64, serial, mSeq), mSeq++);
                        Log.d(TAG, "kim 上传快照响应：" + resp);
                        file.delete();
                        if (resp == null || resp.equals("")) {
                            info = String.format("Wait MSG_DS_POST_SNAP_ACK timeout");
                            showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_WARN, info);
                            return;
                        }

                        JSONObject receivedJson = new JSONObject(resp);
                        final JSONObject easyDarwin = receivedJson.getJSONObject("EasyDarwin");
                        final JSONObject body = easyDarwin.getJSONObject("Body");
                        final JSONObject header = easyDarwin.getJSONObject("Header");
                        int err = header.optInt("ErrorNum");
                        if (err == 200) {
                            info = String.format("Recv MSG_DS_POST_SNAP_ACK");
                            showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);
                        } else {
                            info = String.format("Recv MSG_DS_POST_SNAP_ACK ErrorNum=%d", err);
                            showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_WARN, info);
                        }
                    }
                } catch (Exception e) {
                    info = String.format("Send MSG_DS_POST_SNAP_REQ Fail");
                    showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_WARN, info);
                    e.printStackTrace();
                    Intent intent = new Intent(ACTION_COMMOND_STATE_CHANGED);
                    intent.putExtra(KEY_STATE, STATE_CONNECT_ERROR);
                    LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(intent);
                }
            }
        }
    }

    private void doSendKeepalive() {
        String info = null;
        String ip = EasyApplication.getEasyApplication().getIp();
        String port = EasyApplication.getEasyApplication().getPort();
        String name = EasyApplication.getEasyApplication().getDeviceName();
        String serial = EasyApplication.getEasyApplication().getDeviceSerial();
        String tag = EasyApplication.getEasyApplication().getDeviceTag();
        String token = EasyApplication.getEasyApplication().getDeviceKey();//Token

        if (mOS != null) {
            BlockCommandThread thread = mReadThread;
            if (thread != null) {
                try {
                    String registerJson = String.format("{\n" +
                            "  \"EasyDarwin\": {\n" +
                            "    \"Body\": {\n" +
                            "      \"Name\": \"%s\",\n" +
                            "      \"Serial\": \"%s\",\n" +
                            "      \"Tag\": \"%s\",\n" +
                            "      \"Token\": \"%s\"\n" +
                            "    },\n" +
                            "    \"Header\": {\n" +
                            "      \"AppType\": \"EasyCamera\",\n" +
                            "      \"CSeq\": \"%d\",\n" +
                            "      \"MessageType\": \"MSG_DS_REGISTER_REQ\",\n" +
                            "      \"TerminalType\": \"Android\",\n" +
                            "      \"Version\": \"1.0\"\n" +
                            "    }\n" +
                            "  }\n" +
                            "}", name, serial, tag, token, mSeq);

                    info = String.format("Send MSG_DS_REGISTER_REQ %s [%s:%s]", serial, ip, port);
                    showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);
                    Log.d(TAG, "kim 发送keepalive");
                    String resp = thread.requestAndResp(registerJson, mSeq++);

                    if (resp == null || resp.equals("")) {
                        info = String.format("Wait MSG_DS_REGISTER_ACK timeout");
                        showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_WARN, info);
                        return;
                    }

                    Log.d(TAG, "kim 发送keepalive响应：" + resp);
                    JSONObject receivedJson = new JSONObject(resp);
                    final JSONObject easyDarwin = receivedJson.getJSONObject("EasyDarwin");
                    final JSONObject body = easyDarwin.getJSONObject("Body");
                    final JSONObject header = easyDarwin.getJSONObject("Header");
                    int err = header.optInt("ErrorNum");
                    if (err == 200) {
                        info = String.format("Recv MSG_DS_REGISTER_ACK 200 OK");
                        showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);
                    } else {
                        info = String.format("Recv MSG_DS_REGISTER_ACK ErrorNum %d", err);
                        showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_WARN, info);

                        Intent i = new Intent(ACTION_COMMOND_STATE_CHANGED);
                        i.putExtra(KEY_STATE, STATE_CONNECT_ERROR);
                        LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(i);
                    }
                } catch (Exception e) {
                    info = String.format("Send MSG_DS_REGISTER_REQ %s [%s:%s] Fail", serial, ip, port);
                    showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_WARN, info);
                    e.printStackTrace();
                    Intent intent = new Intent(ACTION_COMMOND_STATE_CHANGED);
                    intent.putExtra(KEY_STATE, STATE_CONNECT_ERROR);
                    LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(intent);
                }
            }
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                Log.d(TAG, "kim onReceive : " + intent.getAction());
                switch (intent.getAction()) {
                    case MediaStream.ACTION_SNAPSHOT_GOT:
                        AsyncTask.execute(new Runnable() {
                            @Override
                            public void run() {
                                doSendSnap();
                            }
                        });
                        break;
                    case ACTION_COMMOND_STATE_CHANGED:
                        int state = intent.getExtras().getInt(KEY_STATE);
                        if (state == STATE_CONNECT_ERROR) {
                            stopService();
                            stopSelf();
                        }
                        break;
                    default:
                        break;
                }
            }
        };

        IntentFilter filter = new IntentFilter();
        filter.addAction(MediaStream.ACTION_SNAPSHOT_GOT);
        filter.addAction(ACTION_COMMOND_STATE_CHANGED);
        LocalBroadcastManager.getInstance(this).registerReceiver(mReceiver, filter);

        PreferenceManager.getDefaultSharedPreferences(this).registerOnSharedPreferenceChangeListener(this);
        EasyApplication.sMainBus.register(this);
    }

    private void closeSocket() {
        try {
            if (mSoc != null) {
                mSoc.close();
                mSoc = null;
            }
            if (mOS != null) {
                mOS.close();
                mOS = null;
            }
            if (mIS != null) {
                mIS.close();
                mIS = null;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void stopService() {

        if (mKeepAliveThread != null) {
            mKeepAliveThread.interrupt();
            mKeepAliveThread = null;
        }

        Thread thread = mReadThread;
        if (thread != null) {
            mReadThread = null;
            thread.interrupt();
            try {
                if (mSoc != null) {
                    mSoc.close();
                    mSoc = null;
                }

                if (mOS != null) {
                    mOS.close();
                    mOS = null;
                }
                if (mIS != null) {
                    mIS.close();
                    mIS = null;
                }
                thread.join();
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        int result = super.onStartCommand(intent, flags, startId);
        start();
        return result;
    }

    private void start() {
        if (mReadThread == null) {
            mReadThread = new BlockCommandThread();
            mReadThread.start();
        }

        if (mKeepAliveThread == null) {
            mKeepAliveThread = new KeepAliveThread();
            mKeepAliveThread.start();
        }
    }

    @Override
    public void onDestroy() {
        EasyApplication.sMainBus.unregister(this);
        Log.d(TAG, "kim onDestroy begin");
        stopService();
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mReceiver);
        PreferenceManager.getDefaultSharedPreferences(this).unregisterOnSharedPreferenceChangeListener(this);
        super.onDestroy();
        LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(new Intent(ACTION_COMMOND_RESTART_THREAD));
        Log.d(TAG, "kim onDestroy end");
    }

    @Subscribe
    public void onPushOK(PushOK ok) {
        JSONObject body = ok.startStreamingReqBody;
        Object channel = body.opt("Channel");
        Object to = body.opt("To");
        Object protocol = body.opt("Protocol");
        Object reserve = body.opt("Reserve");
        Object serial1 = body.opt("Serial");
        Object server_ip = body.opt("Server_IP");
        Object server_port = body.opt("Server_PORT");
        Object from = body.opt("From");
        Object via = body.opt("Via");
        String respStr = String.format("{\n" +
                "  \"EasyDarwin\": {\n" +
                "    \"Body\": {\n" +
                "      \"Channel\": \"%s\",\n" +
                "      \"From\" : \"%s\",\n" +
                "      \"Protocol\": \"%s\",\n" +
                "      \"Reserve\": \"%s\",\n" +
                "      \"Serial\": \"%s\",\n" +
                "      \"Server_IP\": \"%s\",\n" +
                "      \"Server_PORT\": \"%s\",\n" +
                "      \"To\" : \"%s\",\n" +
                "      \"Via\" : \"%s\"\n" +
                "    },\n" +
                "    \"Header\": {\n" +
                "      \"CSeq\": \"%d\",\n" +
                "      \"ErrorNum\": \"200\",\n" +
                "      \"ErrorString\": \"Success OK\",\n" +
                "      \"MessageType\": \"MSG_DS_PUSH_STREAM_ACK\",\n" +
                "      \"Version\": \"1.0\"\n" +
                "    }\n" +
                "  }\n" +
                "}", channel, to, protocol, reserve, serial1, server_ip, server_port, from, via, ok.seq);
        respStr = buildOutStreamContent(respStr);
        String info = String.format("Send MSG_SD_PUSH_STREAM_ACK RTSP [%s:%d]", body.optString("Server_IP"), body.optInt("Server_PORT"));
        showDbgMsg(StatusInfoView.DbgLevel.DBG_LEVEL_INFO, info);
        final String finalRespStr = respStr;
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                try {
                    synchronized (mOS) {
                        mOS.write(finalRespStr);
                        mOS.flush();
                    }
                } catch (Exception ex) {
                    ex.fillInStackTrace();
                }
            }
        });
    }

    private void showDbgMsg(String level, String data) {
        Intent intent = new Intent(StatusInfoView.DBG_MSG);
        intent.putExtra(StatusInfoView.DBG_LEVEL, level);
        intent.putExtra(StatusInfoView.DBG_DATA, data);
        LocalBroadcastManager.getInstance(CommandService.this).sendBroadcast(intent);
    }
}
