import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.zip.DataFormatException;
import java.util.zip.Inflater;

public class Main {

	//png图片的路径
    static File file = new File("xxx.png");

    /**
     * 图片高度96，宽度大于等于最好128
     * 将PNG图片转换成4阶灰度的格式，将数据打印到控制台
     * @param args
     */
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

                System.out.println("const uint8_t Img[][256] = {");
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
}




