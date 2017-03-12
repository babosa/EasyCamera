package org.easydarwin.updatemgr;

/**
 * Created by Kim on 2016/6/13.
 */
public class RemoteVersionInfo {
    private int versionCode;
    private String versionName;
    private String url;

    public int getVersionCode() {
        return versionCode;
    }

    public String getVersionName() {
        return versionName;
    }

    public void setVersionName(String versionName) {
        this.versionName = versionName;
    }

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }
}
