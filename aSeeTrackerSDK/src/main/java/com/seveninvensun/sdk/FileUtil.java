package com.seveninvensun.sdk;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Environment;
import android.util.Log;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Created by 7invensun on 16/12/6.
 */

/**
 * 文件辅助工具，提供注视点标定数据的保存、读取和BMP文件流的封装。
 */
public class FileUtil {

    private static final String TAG = FileUtil.class.getSimpleName();
    /**
     * 写入assets文件夹中的文件到指定目录
     * @param mContext
     * @param assetsFilePath
     * @param targetDirectory
     * @param targetFileName
     * @return
     */
    private static File writeRawFileIfNotExist(Context mContext, String assetsFilePath, File targetDirectory,
                                                 String targetFileName) throws IOException {
        File file = new File(targetDirectory, targetFileName);
        if (file.exists()) {
            file.delete();
        }
        file.createNewFile();
        byte[] buffer = new byte[1024];
        int byteCount = 0;

        AssetManager am = mContext.getAssets();
        InputStream inputStream = am.open(assetsFilePath, AssetManager.ACCESS_BUFFER);
        FileOutputStream out = new FileOutputStream(file);
        while ((byteCount = inputStream.read(buffer, 0, buffer.length)) != -1) {
            out.write(buffer, 0, byteCount);
        }
        out.flush();
        out.close();
        inputStream.close();
        return file;
    }

    /**
     * 写入assets文件夹中的文件到私有目录
     *
     * @param mContext
     * @param directoryName
     *            私有目录创建的文件夹
     * @param assetsFilePath
     *            assets中的文件路径
     * @param fileName
     *            写入后生成的新文件的文件名
     * @return 写入的文件夹，如果写入失败返回null
     */
    static File writeRawFileToPrivateDirectoryIfNotExist(Context mContext, String directoryName,
                                                                String assetsFilePath, String fileName) throws IOException {
        File directory = mContext.getDir(directoryName, Context.MODE_PRIVATE);
        writeRawFileIfNotExist(mContext, assetsFilePath, directory, fileName);
        return directory;
    }

    /**
     * 获得BMP文件头
     * @param size
     * @return
     */
    private static byte[] addBMPImageHeader(int size) {
        byte[] buffer = new byte[14];
        buffer[0] = 0x42; // BmpFileHeader-bfType
        buffer[1] = 0x4D;
        buffer[2] = (byte) (size >> 0); // BmpFileHeader-bfSize
        buffer[3] = (byte) (size >> 8);
        buffer[4] = (byte) (size >> 16);
        buffer[5] = (byte) (size >> 24);
        buffer[6] = 0x00; // BmpFileHeader-bfReserved1
        buffer[7] = 0x00;
        buffer[8] = 0x00; // BmpFileHeader-bfReserved2
        buffer[9] = 0x00;
        buffer[10] = 0x36; // BmpFileHeader-bfOffBits
        buffer[11] = 0x04;
        buffer[12] = 0x00;
        buffer[13] = 0x00;
        return buffer;
    };

    /**
     * 获得BMP文件信息头
     * @param w
     * @param h
     * @param bitCount
     * @param imageSize
     * @return
     */
    private static byte[] addBMPImageInfosHeader(int w, int h, int bitCount, int imageSize) {
        byte[] buffer = new byte[40];
        buffer[0] = 0x28; // BmpInfoHeader-biSize
        buffer[1] = 0x00;
        buffer[2] = 0x00;
        buffer[3] = 0x00;
        buffer[4] = (byte) (w >> 0); // BmpInfoHeader-biWidth
        buffer[5] = (byte) (w >> 8);
        buffer[6] = (byte) (w >> 16);
        buffer[7] = (byte) (w >> 24);
        buffer[8] = (byte) (h >> 0); // BmpInfoHeader-biHeight
        buffer[9] = (byte) (h >> 8);
        buffer[10] = (byte) (h >> 16);
        buffer[11] = (byte) (h >> 24);
        buffer[12] = 0x01; // BmpInfoHeader-biPlanes
        buffer[13] = 0x00;
        buffer[14] = (byte) (bitCount >> 0);// BmpInfoHeader-biBitCount
        buffer[15] = (byte) (bitCount >> 8);
        buffer[16] = 0x00; // BmpInfoHeader-biCompression
        buffer[17] = 0x00;
        buffer[18] = 0x00;
        buffer[19] = 0x00;
        buffer[20] = (byte) (imageSize >> 0);// BmpInfoHeader-biSizeImage
        buffer[21] = (byte) (imageSize >> 8);
        buffer[22] = (byte) (imageSize >> 16);
        buffer[23] = (byte) (imageSize >> 24);
        buffer[24] = 0x00; // BmpInfoHeader-biXPelsPerMeter
        buffer[25] = 0x00;
        buffer[26] = 0x00;
        buffer[27] = 0x00;
        buffer[28] = 0x00; // BmpInfoHeader-biYPelsPerMeter
        buffer[29] = 0x00;
        buffer[30] = 0x00;
        buffer[31] = 0x00;
        buffer[32] = 0x00; // BmpInfoHeader-biClrUsed
        buffer[33] = 0x00;
        buffer[34] = 0x00;
        buffer[35] = 0x00;
        buffer[36] = 0x00; // BmpInfoHeader-biClrImportant
        buffer[37] = 0x00;
        buffer[38] = 0x00;
        buffer[39] = 0x00;
        return buffer;
    };

    /**
     * 获得BMP文件RGBQUAD
     * @return
     */
    private static byte[] addBMPImageRGBQUAD() {
        byte[] buffer = new byte[4 * 256];
        for (int i = 0; i < 256; ++i) {
            buffer[4 * i] = (byte) i;
            buffer[4 * i + 1] = (byte) i;
            buffer[4 * i + 2] = (byte) i;
            buffer[4 * i + 3] = (byte) 0;
        }
        return buffer;
    };

    /**
     * 将bitmap字节流保存成BMP文件
     * @param image
     * @param width
     * @param height
     * @param file
     */
    private static void saveToFile(byte[] image, int width, int height, File file) {
        try {
            if (file.exists()) {
                file.delete();
            }
            file.createNewFile();

            FileOutputStream out = new FileOutputStream(file);

            int bitCount = 8;
            int lineByte = ((width * bitCount >> 3) + 3) / 4 * 4;
            int bufSize = lineByte * height;
            int rgbQuadSize = 4 * 256;
            int bfSize = 54 + rgbQuadSize + bufSize;

            out.write(addBMPImageHeader(bfSize), 0, 14);
            out.write(addBMPImageInfosHeader(width, -height, bitCount, bufSize), 0, 40);
            out.write(addBMPImageRGBQUAD(), 0, rgbQuadSize);
            out.write(image, 0, width * height);
            out.flush();
            out.close();

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * 将Bitmap字节流保存成BMP文件
     * @param image Bitmap文件字节流数组，注意不要包含文件信息，保存时会自动添加文件头信息。
     * @param width Bitmap的宽度
     * @param height Bitmap的高度
     * @param dir BMP图片保存目录
     * @param bmpName BMP图片保存的文件名
     */
    public static void saveImage(byte[] image, int width, int height, File dir, String bmpName) {

        File file = new File(dir, bmpName);
        saveToFile(image, width, height, file);
    }

    private static File createPrivateDirectory(Context context, String name) {
        File directory;
        if (Environment.getExternalStorageState() == Environment.MEDIA_MOUNTED) {
            File sdCard = Environment.getExternalStorageDirectory();
            directory = new File(sdCard, name);
        } else {
            File DownloadDir=Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
            if(DownloadDir.exists()){
                directory=new File(DownloadDir,name);
            }else{
                File cacheDirectory = context.getCacheDir();
                directory = new File(cacheDirectory, name);
            }
        }
        if (directory != null) {
            if (directory.exists()) {
                if (!directory.isDirectory()) {
                    directory.delete();
                }
            }
            directory.mkdirs();
        }
        Log.e(TAG, "标定结果存储在"+directory.getAbsolutePath());
        return directory;
    }

    /**
     * 保存标定数据到指定文件
     * @param mContext Android执行环境
     * @param data 标定数据
     * @param targetFileName 指定文件
     * @return 是否保存成功
     */
    public static boolean writeData(Context mContext, byte[] data, String targetFileName) {
        return writeData(mContext, data, createPrivateDirectory(mContext, "aSeeSDKCalibrationResult"), targetFileName);
    }

    /**
     * 从指定文件读取标定数据
     * @param mContext Android执行环境
     * @param targetFileName 指定文件
     * @return 标定参数数据
     */
    public static byte[] readData(Context mContext, String targetFileName) {
        return readData(mContext, createPrivateDirectory(mContext, "aSeeSDKCalibrationResult"), targetFileName);
    }

    /**
     * 保存标定数据到指定文件
     * @param mContext
     * @param data 标定数据
     * @param targetDirectory 指定目录
     * @param targetFileName 指定文件
     * @return 是否保存成功
     */
    public static boolean writeData(Context mContext, byte[] data, File targetDirectory, String targetFileName) {
        Log.e(TAG, data.length+"");
        File file = new File(targetDirectory, targetFileName);
        try {
            if (file.exists()) {
                file.delete();
            }
            file.createNewFile();
            byte[] buffer = new byte[1024];
            int byteCount = 0;
            InputStream inputStream = new ByteArrayInputStream(data);
            FileOutputStream out = new FileOutputStream(file);
            while ((byteCount = inputStream.read(buffer, 0, buffer.length)) != -1) {
                out.write(buffer, 0, byteCount);
            }
            out.flush();
            out.close();
            inputStream.close();
            return true;
        } catch (FileNotFoundException e) {
            // e.printStackTrace();
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        } catch (IOException e) {
            // e.printStackTrace();
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        }
        return false;
    }

    /**
     * 从指定目录的指定文件读取标定数据
     * @param mContext
     * @param targetDirectory 指定目录
     * @param targetFileName 指定文件
     * @return 标定数据
     */
    public static byte[] readData(Context mContext, File targetDirectory, String targetFileName) {
        File file = new File(targetDirectory, targetFileName);
        try {
            if (file.exists()) {
                FileInputStream inputStream = new FileInputStream(file);
                ByteArrayOutputStream out = new ByteArrayOutputStream();
                byte[] buffer = new byte[1024];
                int byteCount = 0;
                while ((byteCount = inputStream.read(buffer, 0, buffer.length)) != -1) {
                    out.write(buffer, 0, byteCount);
                }
                out.flush();
                out.close();
                inputStream.close();
                byte[] result=out.toByteArray();
                Log.e(FileUtil.class.getSimpleName(), result.length+"");
                return result;
            }
        } catch (FileNotFoundException e) {
            // e.printStackTrace();
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        } catch (IOException e) {
            // e.printStackTrace();
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        }
        return null;
    }
}
