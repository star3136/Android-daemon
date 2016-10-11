package daemon;

import android.app.Service;
import android.content.Context;
import android.util.Log;

import java.io.File;
import java.io.IOException;

/**
 * Created by Allen on 2016/10/10.
 */
public class Daemon {
    private static final String BINARY_PATH = "bin";
    private static final String BINARY_NAME = "daemon";

    private static void execute(Context context, Class<? extends Service> cls, int interval) {
        File file = new File(context.getDir(BINARY_PATH, Context.MODE_PRIVATE), BINARY_NAME);
        String cmd = file.getAbsolutePath();
        String packageName = context.getApplicationContext().getPackageName();
        String serviceName = cls.getName();
        try {
            String cmdLine = cmd + " " + packageName + " " + serviceName + " " + interval;
            Log.e("LEE", cmdLine);
            Runtime.getRuntime().exec(cmdLine).waitFor();
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     *
     * @param context
     * @param cls 需要守护的serice的类
     * @param interval 轮询的毫秒数
     */
    public static void install(final Context context, final Class<? extends Service> cls, final int interval){
        new Thread(){
            @Override
            public void run() {
                Command.install(context, BINARY_PATH, BINARY_NAME);
                execute(context, cls, interval);
            }
        }.start();
    }
}
