/*
	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/

package org.easydarwin.easycamera;

import android.content.DialogInterface;
import android.preference.PreferenceManager;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Toast;

import org.easydarwin.config.Config;
import org.easydarwin.push.MediaStream;

public class SettingActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);
        final EditText txtIp = (EditText) findViewById(R.id.edt_server_address);
        final EditText txtPort = (EditText) findViewById(R.id.edt_server_port);
        final EditText txtId = (EditText) findViewById(R.id.edt_stream_id);
        final EditText txtDeviceSerial = (EditText) findViewById(R.id.edt_device_serial);
        final EditText txtDeviceTag = (EditText) findViewById(R.id.edt_device_tag);
        final EditText txtDeviceToken = (EditText) findViewById(R.id.edt_device_key);
        final EditText txtDeviceName = (EditText) findViewById(R.id.edt_device_name);
        final EditText txtKeepAliveIntervall = (EditText) findViewById(R.id.edt_keep_alive_interval);

        String ip = EasyApplication.getEasyApplication().getIp();
        String port = EasyApplication.getEasyApplication().getPort();
        String deviceSerial = EasyApplication.getEasyApplication().getDeviceSerial();
        String deviceTag = EasyApplication.getEasyApplication().getDeviceTag();
        String deviceToken = EasyApplication.getEasyApplication().getDeviceKey();
        String deviceName = EasyApplication.getEasyApplication().getDeviceName();
        String keepAliveInterval = EasyApplication.getEasyApplication().getKeepAliveInterval();

        txtIp.setText(ip);
        txtPort.setText(port);
        txtDeviceSerial.setText(deviceSerial);
        txtDeviceTag.setText(deviceTag);
        txtDeviceToken.setText(deviceToken);
        txtDeviceName.setText(deviceName);
        txtKeepAliveIntervall.setText(keepAliveInterval);

        CheckBox x264enc = (CheckBox) findViewById(R.id.use_x264_encode);
        x264enc.setChecked(PreferenceManager.getDefaultSharedPreferences(this).getBoolean("key-sw-codec", MediaStream.useSWCodec()));

        x264enc.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                PreferenceManager.getDefaultSharedPreferences(SettingActivity.this).edit().putBoolean("key-sw-codec", isChecked).apply();
            }
        });
        CheckBox backgroundPushing = (CheckBox) findViewById(R.id.enable_background_camera_pushing);
        backgroundPushing.setChecked(PreferenceManager.getDefaultSharedPreferences(this).getBoolean("key_enable_background_camera", true));

        backgroundPushing.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                PreferenceManager.getDefaultSharedPreferences(SettingActivity.this).edit().putBoolean("key_enable_background_camera", isChecked).apply();
            }
        });


        Button btnSave = (Button) findViewById(R.id.btn_save);
        btnSave.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String ipValue = txtIp.getText().toString();
                if (TextUtils.isEmpty(ipValue)) {
                    ipValue = Config.DEFAULT_SERVER_IP;
                }
                EasyApplication.getEasyApplication().saveStringIntoPref(Config.SERVER_IP, ipValue);

                String portValue = txtPort.getText().toString();
                if (TextUtils.isEmpty(portValue)) {
                    portValue = Config.DEFAULT_SERVER_PORT;
                }
                EasyApplication.getEasyApplication().saveStringIntoPref(Config.SERVER_PORT, portValue);

                String deviceSerialValue = txtDeviceSerial.getText().toString();
                if (TextUtils.isEmpty(deviceSerialValue)) {
                    deviceSerialValue = System.currentTimeMillis()+"";
                }
                EasyApplication.getEasyApplication().saveStringIntoPref(Config.device_serial_key, deviceSerialValue);

                String deviceTagValue = txtDeviceTag.getText().toString();
                if (TextUtils.isEmpty(deviceTagValue)) {
                    deviceTagValue = Config.device_tag_default_value;
                }
                EasyApplication.getEasyApplication().saveStringIntoPref(Config.device_tag_key, deviceTagValue);

                String deviceTokenValue = txtDeviceToken.getText().toString();
                if (TextUtils.isEmpty(deviceTokenValue)) {
                    deviceTokenValue = Config.device_key_default_value;
                }
                EasyApplication.getEasyApplication().saveStringIntoPref(Config.device_name_key, deviceTokenValue);

                String deviceNameValue = txtDeviceName.getText().toString();
                if (TextUtils.isEmpty(deviceNameValue)) {
                    deviceNameValue = Config.device_name_default_value;
                }
                EasyApplication.getEasyApplication().saveStringIntoPref(Config.device_name_key, deviceNameValue);

                String keepAliveIntervalValue =txtKeepAliveIntervall.getText().toString();
                if (TextUtils.isEmpty(keepAliveIntervalValue)) {
                    keepAliveIntervalValue = Config.keep_alive_interval_default_value;
                }
                EasyApplication.getEasyApplication().saveStringIntoPref(Config.keep_alive_interval_key, keepAliveIntervalValue);




                onBackPressed();
            }
        });

        final CharSequence[] items = {"低", "普通", "高", "很高", "极高"};
        int mode = PreferenceManager.getDefaultSharedPreferences(SettingActivity.this).getInt("aec_mode", 1);
        Button btn = (Button) findViewById(R.id.aec_mode);
        btn.setText(String.format("回升抵消级别:%s", items[mode]));
    }

    private void showToast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_LONG).show();
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }

    public void onAECMode(final View view) {
        final CharSequence[] items = {"低", "普通", "高", "很高", "极高"};
        new AlertDialog.Builder(this).setTitle("回音抵消级别").setItems(items, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                PreferenceManager.getDefaultSharedPreferences(SettingActivity.this).edit().putInt("aec_mode", i).apply();
                Button btn = (Button) view;
                btn.setText(String.format("回升抵消级别:%s", items[i]));
            }
        }).show();

    }
}
