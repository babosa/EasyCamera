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
import android.media.audiofx.AudioEffect;
import android.media.audiofx.EnvironmentalReverb;
import android.media.audiofx.LoudnessEnhancer;
import android.os.Build;
import android.util.Base64;
import android.util.Log;

import com.squareup.otto.Bus;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import static org.easydarwin.easycamera.EasyApplication.sMainBus;

public class TalkPlay {
    private final static String TAG = "TalkPlay";
    Context mContext;
    public AudioManager.OnAudioFocusChangeListener l;
    public Object[] mAes = new Object[10];
    public AudioTrack mAudioTrack;
    private final Bus bus;


    private void deleteAudioTrack() {
        for (Object ae : mAes) {
            if (ae != null) {
                AudioEffect aet = (AudioEffect) ae;
                aet.release();
            }
        }
        mAudioTrack.release();
        AudioManager am = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
        am.abandonAudioFocus(l);
    }

    private void initAudioTrack() {
        AudioManager am = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
        l = new AudioManager.OnAudioFocusChangeListener() {
            @Override
            public void onAudioFocusChange(int focusChange) {
                if (focusChange == AudioManager.AUDIOFOCUS_GAIN) {
                    AudioTrack audioTrack = mAudioTrack;
                    if (audioTrack != null) {
                        audioTrack.setStereoVolume(1.0f, 1.0f);
                        if (audioTrack.getPlayState() == AudioTrack.PLAYSTATE_PAUSED) {
                            audioTrack.flush();
                            audioTrack.play();
                        }
                    }
                } else if (focusChange == AudioManager.AUDIOFOCUS_LOSS) {
                    AudioTrack audioTrack = mAudioTrack;
                    if (audioTrack != null) {
                        if (audioTrack.getPlayState() == AudioTrack.PLAYSTATE_PLAYING) {
                            audioTrack.pause();
                        }
                    }
                } else if (focusChange == AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK) {
                    AudioTrack audioTrack = mAudioTrack;
                    if (audioTrack != null) {
                        audioTrack.setStereoVolume(0.5f, 0.5f);
                    }
                }
            }
        };

        int requestCode = am.requestAudioFocus(l, AudioManager.STREAM_VOICE_CALL, AudioManager.AUDIOFOCUS_GAIN);

        if (requestCode != AudioManager.AUDIOFOCUS_REQUEST_GRANTED) {
            Log.w(TAG, String.format("requestAudioFocus result:%d", requestCode));
        }

        final int sampleRateInHz = 8000;
        final int channelConfig = AudioFormat.CHANNEL_OUT_MONO;
        final int bfSize = (int) (AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig, AudioFormat.ENCODING_PCM_16BIT));
        am.setSpeakerphoneOn(true);
        // 10毫秒内的字节数
        final int unit_length = sampleRateInHz * 10 / 1000 * 2;

        mAudioTrack = new AudioTrack(AudioManager.STREAM_VOICE_CALL, sampleRateInHz, channelConfig,
                AudioFormat.ENCODING_PCM_16BIT, bfSize * 4, AudioTrack.MODE_STREAM);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            int i = 0;
            try {
                EnvironmentalReverb er = new EnvironmentalReverb(0, mAudioTrack.getAudioSessionId());
                if (er != null) {
                    er.setEnabled(true);
                    mAes[i++] = er;
                }
            } catch (Throwable ex) {
                ex.printStackTrace();
            }
            try {
                LoudnessEnhancer le = new LoudnessEnhancer(mAudioTrack.getAudioSessionId());
                le.setEnabled(true);
                mAes[i++] = le;
            } catch (Throwable ex) {
                ex.printStackTrace();
            }
        }
        mAudioTrack.play();
    }
    public TalkPlay(Context context){
        this.mContext = context;
        bus = sMainBus;
    }

    public void inputDataToQueue(String pts, String data){
        byte[] base64data = Base64.decode(data,Base64.NO_WRAP);
        int len = base64data.length * 2;
        int left = len % 320;
        if (left != 0){
            len += 320;
            len -= left;
        }
        byte[] decodedData = new byte[len];

        G711A.decode(base64data, 0, base64data.length, decodedData);
        ByteBuffer buffer = ByteBuffer.wrap(decodedData, 0, 0);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        do {
            if (buffer.limit() >= buffer.capacity()){
                break;
            }
            buffer.limit(buffer.limit() + 320);
            long begin = System.currentTimeMillis();
            int r;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                r = mAudioTrack.write(buffer.array(), buffer.position(), buffer.remaining(), AudioTrack.WRITE_NON_BLOCKING);
            } else {
                r = mAudioTrack.write(buffer.array(), buffer.position(), buffer.remaining());
            }
            Log.d(TAG, String.format("mAudioTrack write ret:%d,spend:%d", r, System.currentTimeMillis() - begin));
            bus.post(buffer);
            buffer.position(buffer.limit());
        } while (mAudioTrack != null);
    }

    public void startPlay(){
        Log.d(TAG, "kim startPlay");

        initAudioTrack();

    }

    public void stopPlay(){
        Log.d(TAG, "kim stopPlay");
        deleteAudioTrack();
    }
}
