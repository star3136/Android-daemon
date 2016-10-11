package daemon;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Build;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by Allen on 2016/10/10.
 */
public class Command {
    public static boolean install(Context context, String dirpath, String filename) {
        File file = new File(context.getDir(dirpath, Context.MODE_PRIVATE), filename);
        if(file.exists()) {
            return false;
        }
        String eabiDir = "armeabi";
        String abi = Build.CPU_ABI;
        if (abi.startsWith("armeabi-v7a")) {
            eabiDir = "armeabi-v7a";
        } else if (abi.startsWith("x86")) {
            eabiDir = "x86";
        }
        try {
            copyAssets(context, eabiDir + File.separator + filename, file, "755");
            return true;
        }catch (Exception e) {

            return false;
        }

    }

    private static void copyAssets(Context context, String filePath, File f, String mode) throws Exception {
        String absPath = f.getAbsolutePath();
        AssetManager assetManager = context.getAssets();
        InputStream is = assetManager.open(filePath);
        OutputStream os = new FileOutputStream(f);
        byte[] buf = new byte[4096];

        int len;
        while ((len = is.read(buf)) > 0) {
            os.write(buf, 0, len);
        }
        os.close();
        is.close();

        Runtime.getRuntime().exec("chmod " + mode + " " + absPath).waitFor();
    }
}
