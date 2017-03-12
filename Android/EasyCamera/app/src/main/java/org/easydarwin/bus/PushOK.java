/*
	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.bus;

import org.json.JSONObject;

public class PushOK {
    public final int seq;
    public final JSONObject startStreamingReqBody;

    public PushOK(JSONObject startStreamingReqBody, int seq) {
        this.startStreamingReqBody = startStreamingReqBody;
        this.seq = seq;
    }
}
