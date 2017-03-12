package org.easydarwin.easycamera;

import android.app.Application;
import android.content.SharedPreferences;
import android.hardware.Camera;
import android.preference.PreferenceManager;
import android.text.TextUtils;

import com.android.webrtc.audio.AudioIO;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.squareup.otto.Bus;

import org.easydarwin.config.Config;
import org.easydarwin.util.Util;

import java.io.IOException;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;

import okhttp3.FormBody;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

/**
 * Created by Helong on 16/4/16-12:54.
 */
public class EasyApplication extends Application {

    private static EasyApplication mApplication;
    public static AudioIO aio;
    public static Bus sMainBus;

    @Override
    public void onCreate() {
        super.onCreate();
        mApplication = this;
        if (Util.getSupportResolution(this).size() == 0) {
            StringBuilder stringBuilder = new StringBuilder();
            Camera camera = Camera.open();
            List<Camera.Size> supportedPreviewSizes = camera.getParameters().getSupportedPreviewSizes();
            for (Camera.Size str : supportedPreviewSizes) {
                stringBuilder.append(str.width + "x" + str.height).append(";");
            }
            Util.saveSupportResolution(this, stringBuilder.toString());
            camera.release();
        }

        initSystemConfig();
        aio = new AudioIO(this, 8000, false);
        sMainBus = new Bus();
    }

    private void initSystemConfig(){
        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        String deviceSerial=sharedPreferences.getString(Config.device_serial_key,"");
        if(TextUtils.isEmpty(deviceSerial)){
            deviceSerial=System.currentTimeMillis()+"";
            editor.putString(Config.device_serial_key,deviceSerial);
            editor.commit();
        }

        String deviceName=sharedPreferences.getString(Config.device_name_key,"");
        if(TextUtils.isEmpty(deviceName)){
            editor.putString(Config.device_name_key,Config.device_name_default_value);
            editor.commit();
        }

        String deviceKey=sharedPreferences.getString(Config.device_key_key,"");
        if(TextUtils.isEmpty(deviceKey)){
            editor.putString(Config.device_key_key,Config.device_key_default_value);
            editor.commit();
        }

        String deviceTag=sharedPreferences.getString(Config.device_tag_key,"");
        if(TextUtils.isEmpty(deviceTag)){
            editor.putString(Config.device_tag_key,Config.device_tag_default_value);
            editor.commit();
        }

        String deviceKeepAliveInterval=sharedPreferences.getString(Config.keep_alive_interval_key,"");
        if(TextUtils.isEmpty(deviceKeepAliveInterval)){
            editor.putString(Config.keep_alive_interval_key,Config.keep_alive_interval_default_value);
            editor.commit();
        }
    }

    public static EasyApplication getEasyApplication() {
        return mApplication;
    }

    public void saveStringIntoPref(String key, String value) {
        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putString(key, value);
        editor.commit();
    }

    public String getDeviceSerial(){
        return PreferenceManager.getDefaultSharedPreferences(this)
                .getString(Config.device_serial_key,String.valueOf(System.currentTimeMillis()));
    }

    public String getDeviceName(){
        return PreferenceManager.getDefaultSharedPreferences(this)
                .getString(Config.device_name_key,Config.device_name_default_value);
    }

    public String getDeviceKey(){
        return PreferenceManager.getDefaultSharedPreferences(this)
                .getString(Config.device_key_key,Config.device_key_default_value);
    }

    public String getDeviceTag(){
        return PreferenceManager.getDefaultSharedPreferences(this)
                .getString(Config.device_tag_key,Config.device_tag_default_value);
    }

    public String getKeepAliveInterval(){
        return PreferenceManager.getDefaultSharedPreferences(this)
                .getString(Config.keep_alive_interval_key,Config.keep_alive_interval_default_value);
    }

    public String getIp() {
        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        String ip = sharedPreferences.getString(Config.SERVER_IP, Config.DEFAULT_SERVER_IP);
        return ip;
    }

    public String getPort() {
        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        String port = sharedPreferences.getString(Config.SERVER_PORT, Config.DEFAULT_SERVER_PORT);
        return port;
    }

//    public String getId() {
//        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
//        String id = sharedPreferences.getString(Config.STREAM_ID, Config.DEFAULT_STREAM_ID);
//        if (!id.contains(Config.STREAM_ID_PREFIX)) {
//            id = Config.STREAM_ID_PREFIX + id;
//        }
//        saveStringIntoPref(Config.STREAM_ID, id);
//        return id;
//    }






    private static final OkHttpClient client = new OkHttpClient();

    public static String get(String url) throws IOException {
        Request request = new Request.Builder()
                .url(url)
                .build();

        Response response = client.newCall(request).execute();
        return response.body().string();
    }

    public static void asyncGet(String url, CallbackWrapper callback) {
        Request request = new Request.Builder()
                .url(url)
                .build();

        client.newCall(request).enqueue(callback);
        callback.onBefore(request);
    }


    public static void asyncPost(String url, String content, CallbackWrapper callback) {
        MediaType MEDIA_TYPE_PLAIN = MediaType.parse("text/plain;charset=utf-8");


        Request request = new Request.Builder()
                .url(url)
                .post(RequestBody.create(MEDIA_TYPE_PLAIN, content))
                .build();

        client.newCall(request).enqueue(callback);
        callback.onBefore(request);
    }

    public static void asyncPost(String url, CallbackWrapper callback) {
        FormBody.Builder builder = new FormBody.Builder();
        Request request = new Request.Builder()
                .url(url)
                .post(builder.build())
                .build();

        client.newCall(request).enqueue(callback);
        callback.onBefore(request);
    }

    public static FutureTask<JsonObject> syncGet(final String url) {
        return new FutureTask<>(new Callable<JsonObject>() {
            @Override
            public JsonObject call() throws Exception {
                Request request = new Request.Builder()
                        .url(url)
                        .build();

                return parseBody(client.newCall(request).execute());
            }
        });
    }


    public static JsonObject parseBody(Response response) throws IOException {
        if (response.isSuccessful()) {
            JsonObject o = new JsonParser().parse(response.body().string()).getAsJsonObject();
            JsonObject EasyDarwin = o.getAsJsonObject("EasyDarwin");
            JsonObject header = EasyDarwin.getAsJsonObject("Header");
            final int code = header.getAsJsonPrimitive("ErrorNum").getAsInt();
            final String error = header.getAsJsonPrimitive("ErrorString").getAsString();
            if (code == 200) {
                return EasyDarwin.getAsJsonObject("Body");
            }
            throw new CallbackWrapper.HttpCodeErrorExcepetion(code, error);
        }
        throw new CallbackWrapper.HttpCodeErrorExcepetion(response.code());
    }
}
