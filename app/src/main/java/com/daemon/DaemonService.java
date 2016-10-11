package com.daemon;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;

import daemon.Daemon;

/**
 * Created by Allen on 2016/10/11.
 */
public class DaemonService extends Service {
    @Override
    public void onCreate() {
        super.onCreate();

        Daemon.install(this, DaemonService.class, 5000);
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}
