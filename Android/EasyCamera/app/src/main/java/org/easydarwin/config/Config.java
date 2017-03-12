/*
	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/

package org.easydarwin.config;

import android.os.Build;

/**
 * 类Config的实现描述：
 */
public class Config {

    public static final String SERVER_IP = "serverIp";
    public static final String SERVER_PORT = "serverPort";
    public static final String DEFAULT_SERVER_IP = "121.40.50.44";
    public static final String DEFAULT_SERVER_PORT = "10000";
    public static final String PREF_NAME = "easy_pref";
    public static final String K_RESOLUTION = "k_resolution";

    /*设备序列号，默认系统时间*/
    public static final String device_serial_key="device_serial";

    public static final String device_name_key="device_name_key";
    public static final String device_name_default_value=Build.MODEL.replace(" ","");

    public static final String device_key_key="device_key_key";
    public static final String device_key_default_value="000000";

    public static final String device_tag_key="device_tag_key";
    public static final String device_tag_default_value="Android";

    public static final String keep_alive_interval_key="keep_alive_interval_key";
    public static final String keep_alive_interval_default_value="30";

}
