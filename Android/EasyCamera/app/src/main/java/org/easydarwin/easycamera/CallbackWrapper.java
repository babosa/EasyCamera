/*
	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easycamera;

import android.os.Handler;
import android.util.Log;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;


import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Request;
import okhttp3.Response;

/**
 * Created by Helong on 16/3/15-22:26.
 */
public class CallbackWrapper<T> implements Callback {

    public static final class HttpCodeErrorExcepetion extends IOException{
        private final int mCode;

        public HttpCodeErrorExcepetion(int code){
            super(String.valueOf(code));
            mCode = code;
        }

        public HttpCodeErrorExcepetion(int code, String error) {
            super(error);
            mCode = code;
        }
    }
    private final Class<T> mClass;

    public CallbackWrapper(Class<T> typeParameterClass){
        mClass = typeParameterClass;
    }

    private static final Handler sHandler = new Handler();

    @Override
    public void onFailure(final Call call, final IOException e) {
        sHandler.post(new Runnable() {
            @Override
            public void run() {
                onError(call, e);
                onAfter();
            }
        });
    }

    @Override
    public void onResponse(final Call call, final Response response) throws IOException {
        if (!response.isSuccessful()){
            sHandler.post(new Runnable() {
                @Override
                public void run() {
                    onError(call, new HttpCodeErrorExcepetion(response.code()));
                    onAfter();
                }
            });
            return;
        }
        String string = response.body().string();
        Log.d("LiveVOCallback","response: "+string);
        JsonObject o = new JsonParser().parse(string).getAsJsonObject();
        JsonObject EasyDarwin = o.getAsJsonObject("EasyDarwin");
        if (EasyDarwin == null){
            final T t = new Gson().fromJson(string, mClass);
            sHandler.post(new Runnable() {
                @Override
                public void run() {
                    onResponse(t);
                    onAfter();
                }
            });
        }else{
            try {
                JsonObject header = EasyDarwin.getAsJsonObject("Header");
                final int code = header.getAsJsonPrimitive("ErrorNum").getAsInt();
                final String error = header.getAsJsonPrimitive("ErrorString").getAsString();

                if (code != 200) {
                    sHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            onError(call, new HttpCodeErrorExcepetion(code, error));
                            onAfter();
                        }
                    });
                }else{
                    JsonObject body = EasyDarwin.getAsJsonObject("Body");
                    final T t = new Gson().fromJson(body.toString(), mClass);
                    sHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            onResponse(t);
                            onAfter();
                        }
                    });
                }
            }catch (final Exception ex){
                ex.printStackTrace();
                sHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        onError(call, ex);
                        onAfter();
                    }
                });
            }

        }
    }


    public void onBefore(Request request) {
    }

    public void onAfter() {

    }

    public void onError(Call call, Exception e) {
    }

    public void onResponse(T obj) {

    }
}
