/*
	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.audio;

import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.AsyncTask;
import android.util.Base64;
import android.util.Log;

import com.android.webrtc.audio.AudioIO;

import org.easydarwin.easycamera.EasyApplication;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class TalkPlay {
    private final static String TAG = "TalkPlay";
    private static final AudioIO aio = EasyApplication.aio;
    Context mContext;

    public TalkPlay(Context context){
        this.mContext = context;
    }

    public void inputDataToQueue(String pts, String data){
        byte[] base64data = Base64.decode(data,Base64.NO_WRAP);
        byte[] decodedData = new byte[base64data.length*2];

        G711A.decode(base64data, 0, base64data.length, decodedData);

//                        Log.d(TAG, "kim dataForDecode.length="+dataForDecode.length+", base64 size="+base64data.length+", decodedData length="+decodedData.length);

        //play
        ShortBuffer shortBuffer = ByteBuffer.wrap(decodedData).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer();

        short []buffer = new short[shortBuffer.remaining()];
        shortBuffer.get(buffer);
        try {
            aio.start();
            aio.pumpAudio(buffer, 0, buffer.length);
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }finally {
            try {
                aio.release();
            } catch (IOException e) {
                e.printStackTrace();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public void startPlay(){
        Log.d(TAG, "kim startPlay");


    }

    public void stopPlay(){
        Log.d(TAG, "kim stopPlay");
    }
}
