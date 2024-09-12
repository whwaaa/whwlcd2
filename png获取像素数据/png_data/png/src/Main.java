import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.zip.DataFormatException;
import java.util.zip.Inflater;

public class Main {

    static File file = new File("C:\\Users\\whw\\Desktop\\myproj\\2.3寸电阻触摸灰阶\\png\\my.png");

    public static void testmain(String[] args) throws IOException {
        File file2 = new File("C:\\Users\\whw\\Desktop\\myproj\\2.3寸电阻触摸灰阶\\png\\2.png");
        // 读取PNG文件
        BufferedImage image = ImageIO.read(file2);

        // 检查图像是否成功读取
        if (image != null) {
            // 获取图像的宽度和高度
            int width = image.getWidth();
            int height = image.getHeight();

            // 打印图像的宽度和高度
            System.out.println("Width: " + width);
            System.out.println("Height: " + height);

            // 获取图像的像素数据
            int[] pixels = new int[width * height];
            image.getRGB(0, 0, width, height, pixels, 0, width);

            System.out.println("读取像素值：");
            for ( int j=0; j<96; j++ ) {
                for ( int i=0; i<width; i++ ) {
                    System.out.printf("0x%02X ", (byte)pixels[j*width + i]);
                }
                System.out.println();
            }
        }
    }

    public static void main(String[] args) {
        try {
            // 读取PNG文件
            BufferedImage image = ImageIO.read(file);

            // 检查图像是否成功读取
            if (image != null) {
                // 获取图像的宽度和高度
                int width = image.getWidth();
                int height = image.getHeight();

                // 打印图像的宽度和高度
                System.out.println("Width: " + width);
                System.out.println("Height: " + height);

                // 获取图像的像素数据
                int[] pixels = new int[width * height];
                byte[] target = new byte[width * height * 2 / 8];// = {};
                for ( int i=0; i<width*height*2/8; i++ ) {
                    target[i] = 0;
                }
                image.getRGB(0, 0, width, height, pixels, 0, width);

                for ( int page=0; page<12; page++ ) {//高96，12页
                    for ( int i=0; i<width; i++ ) {
                        for ( int j=0; j<8; j++ ) {
                            int temp = pixels[ page*8*width + j*width + i ] & 0x000000FF;
                            if ( temp < (int)64 ) {//0~63 暗 位bit1灰度bit1
                                target[page*width*2 + i*2] |= 1<<j;
                                target[page*width*2 + i*2 + 1] |= 1<<j;
                            } else if ( temp < (int)178 ) {//64~127 次暗 位bit1灰度bit0
                                target[page*width*2 + i*2] |= 1<<j;
                                //target[page*width*2 + i*2 + 1] |= 0<<j;
                            } else if ( temp < (int)210 ) {//128~192 次亮 位bit0灰度bit1
                                //target[page*width*2 + i*2] |= 0<<j;
                                target[page*width*2 + i*2 + 1] |= 1<<j;
                            } else {//192~256 亮 位bit0灰度bit0
                                //target[page*width*2 + i*2] |= 0<<j;
                                //target[page*width*2 + i*2 + 1] |= 0<<j;
                            }
                        }
                    }
                }

                System.out.println("uint8_t testImg[][256] = {");
                for ( int page=0; page<12; page++ ) {//高96，12页
                    System.out.print("  { ");
                    for (int i=0; i<width; i++) {
                        System.out.printf("0x%02X,0x%02X,", target[page*width*2 + i*2], target[page*width*2 + i*2+1]);
                    }
                    System.out.println(" },");
                }
                System.out.println("};");

            } else {
                System.out.println("Failed to read the image.");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void testtest(String[] args) throws IOException, DataFormatException {
        byte[] zlibPng = new byte[10240];//10K大小数据
        int len;

        len = getFile(zlibPng);

        for (int i = 0; i < len; i++) {
            System.out.printf("0x%02X,", zlibPng[i]);
        }

        // 解压缩
        byte[] decompressedData = decompress(zlibPng);

        // 输出解压缩后的数据，仅为示例
        System.out.println("\n解压缩后的数据：");
        for (int i = 0; i < decompressedData.length; i++) {
            System.out.printf("0x%02X,", decompressedData[i]);
        }

    }

    public static byte[] decompress(byte[] compressedData) throws IOException, DataFormatException {
        Inflater inflater = new Inflater();
        inflater.setInput(compressedData);

        ByteArrayOutputStream outputStream = new ByteArrayOutputStream(compressedData.length);
        byte[] buffer = new byte[1024];
        while (!inflater.finished()) {
            int count = inflater.inflate(buffer);
            outputStream.write(buffer, 0, count);
        }
        outputStream.close();

        return outputStream.toByteArray();
    }

    public static int getFile(byte[] zlibPng) throws IOException {
        int idx = 0;
        int idatLen = 0;
        // 创建输入流
        FileInputStream fileInputStream = new FileInputStream(file);
        while(true){
            // 创建缓存数组
            byte[] b = new byte[3000];
            int len = fileInputStream.read(b);      // 返回实际读取字节数
            if(len == -1){
                fileInputStream.close();
                break;
            }

            for (int i = 0; i < len; i++) {
                if ( b[i]==0x49 && b[i+1]==0x44 && b[i+2]==0x41 && b[i+3]==0x54 ) {
                    idatLen = b[i-4]<<24 | b[i-3]<<16 | b[i-2]<<8 | b[i-1];
                    System.out.printf("IDAT总长度：%d\n", idatLen);
                    idx = i+4;
                    System.out.print("png中找到IDAT数据块：\n");
                    int k=0;
                    for (int j = idx; j < idatLen+idx; j++) {
//                        System.out.printf("0x%02X,", b[j]);
                        zlibPng[k++] = b[j];
                    }
                    System.out.println();
                    return idatLen;
                }
            }

        }
        return 0;
    }

    public static void compressFile(String sourceFile, String compressdFile) {

    }
}




