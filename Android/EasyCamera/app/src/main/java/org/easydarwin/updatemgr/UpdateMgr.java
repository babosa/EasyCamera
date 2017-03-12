package org.easydarwin.updatemgr;

import android.app.AlertDialog;
import android.app.DownloadManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.text.TextUtils;
import android.util.Log;

import org.easydarwin.easycamera.CallbackWrapper;
import org.easydarwin.easycamera.EasyApplication;
import org.easydarwin.easycamera.R;

import okhttp3.Call;

/**
 * Created by kim on 2016/6/13.
 */
public class UpdateMgr {
    private static final String TAG = "UpdateMgr";
    private Context mContext;

    public UpdateMgr(Context context){
        this.mContext = context;
    }
    /**
     * 检测当前APP是否需要升级
     */
    public void checkUpdate(){
        String url="http://www.easydarwin.org/versions/easycamera/version.txt";

        EasyApplication.asyncGet(url, new CallbackWrapper<RemoteVersionInfo>(RemoteVersionInfo.class) {
            @Override
            public void onError(Call call, Exception e) {
                Log.e(TAG, "Check update faill ...");
            }

            @Override
            public void onResponse(RemoteVersionInfo response) {
                if (response == null && TextUtils.isEmpty(response.getUrl())) {
                    return;
                }
                PackageManager packageManager = mContext.getPackageManager();
                try {
                    PackageInfo packageInfo = packageManager.getPackageInfo(mContext.getPackageName(), 0);
                    int localVersionCode = packageInfo.versionCode;
                    int remoteVersionCode = response.getVersionCode();
                    if (localVersionCode < remoteVersionCode) {
                        showUpdateDialog(response.getUrl());
                    }
                } catch (PackageManager.NameNotFoundException e) {
                    Log.e(TAG, "Get PackageInfo error !");
                }
            }
        });
    }

    /**
     * 提示升级对话框
     */
    private void showUpdateDialog(String linkUrl){
        final String apkUrl=linkUrl;
        Log.d(TAG, "kim showUpdateDialog. apkUrl="+apkUrl);
        new AlertDialog.Builder(mContext)
                .setMessage("EasyCamera可以升级到更高的版本，是否升级")
                .setTitle("升级提示")
                .setIcon(R.drawable.easy_logo)
                .setPositiveButton("升级", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        DownloadManager downloadManager = (DownloadManager) mContext.getSystemService(Context.DOWNLOAD_SERVICE);
                        Uri uri = Uri.parse(apkUrl);
                        DownloadManager.Request request = new DownloadManager.Request(uri);
                        // 设置允许使用的网络类型，这里是移动网络和wifi都可以
                        request.setAllowedNetworkTypes(DownloadManager.Request.NETWORK_MOBILE | DownloadManager.Request.NETWORK_WIFI);
                        // 禁止发出通知，既后台下载，如果要使用这一句必须声明一个权限：android.permission.DOWNLOAD_WITHOUT_NOTIFICATION
                        request.setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE);
                        downloadManager.enqueue(request);
                        dialog.dismiss();
                    }
                }).setNegativeButton("取消", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        }).create().show();
    }
}
