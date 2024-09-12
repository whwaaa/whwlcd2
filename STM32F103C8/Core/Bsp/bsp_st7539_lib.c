#include "bsp_st7539.h"

/**
 * 设置RAM的数据列地址（写入数据前设置）
 * 
 * RAM数据缓存阵列为64+1行192列,只能设置0~191范围
 *     D7  D6  D5  D4  D3  D2  D1  D0
 * LSB:0   0   0   0   CA3 CA2 CA1 CA0
 * MSB:0   0   0   1   CA7 CA6 CA5 CA4
 * column值对应CA[0,191]
 * column取值：0~191
*/
void lcd_set_column_address( uint8_t column ) {
    uint8_t cmd;
    if ( column > 191 )
        column = 0;
    //设置低位
    cmd = column & 0x0F;
    writeByteCmd(cmd);//HAL_I2C_Master_Transmit( &HI2Cx, W_CMD_ADDRESS, &cmd, 1, 1000 );
    //设置高位
    cmd = 0x10 | ( ( column & 0xF0 ) >> 4 );
    writeByteCmd(cmd);//HAL_I2C_Master_Transmit( &HI2Cx, W_CMD_ADDRESS, &cmd, 1, 1000 );
}

/**
 * 设置屏幕显示区顶部所在的RAM数据行
 * 
 * RAM数据缓存阵列为64+1行192列，若此处设置line=x，x可
 * 设置范围[0,63]，则屏幕将显示RAM的缓存数据，从x行到
 * x+32行(屏幕分辨率是128*32)。
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 0   1   SL5 SL4 SL3 SL2 SL1 SL0
 * 可设置范围SL[5:0] : [0,63]
 * 
 * line取值：0~63
*/
void lcd_set_scroll_line( uint8_t line ) {
    if ( line > 0x3F ) 
        line = 0;//RST硬件复位默认值
    writeByteCmd(0x40|line);
}

/**
 * 设置RAM的数据页地址（写入数据前设置）
 * 
 * RAM缓存阵列分布64+1行*192列，其中65行中分成8+1页，
 * 前8页page0~page7每页有8行，最后一行page8单行用做
 * icon。指定RAM的页地址后，每写1字节(8bit)数据，则
 * RAM在对应页1个列和8个行的位置存入数据：
 * 
 * 例：在page0的第0列写入数据0xFE，在RAM中对应的位置如下：
 *        ------------------...共192列 
 *        0
 *        1
 *        1
 *        1
 *        1
 *        1
 *        1
 *        1
 *        ------------------...共192列
 * 
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   0   1   1   PA3 PA2 PA1 PA0
 * page取值：0~8（page8是icon RAM，只有1行，写入数据最低为有效）
 * 
*/
void lcd_set_page_address( uint8_t page ) {
    if ( page > 8 )
        page = 0;//RST硬件复位默认值
    writeByteCmd(0xB0|page);
}

/**
 * 设置对比度
 * 
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   0   0   0   0   0   0   1
 * EV7 EV6 EV5 EV4 EV3 EV2 EV1 EV0
 * 要发2字节，发第一字节表示开始设置，发第二字节是具体对比度的值
 * 对比度值设置越大背景越亮，值设置越小背景越暗(雪花纹越明显)
 * 
 * contraset取值0~255
*/
void lcd_set_contraset( uint8_t contraset ) {
    writeByteCmd(0x81);
    writeByteCmd(contraset);
}

/**
 * 部分屏模式
 * 
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   0   0   0   0   1   0   PS
 * PS=0: 全显示模式
 * PS=1: 部分显示模式
 * ps取值：0~1
*/
void lcd_set_partial_screen_mode( uint8_t ps ) {
    if ( ps > 1 ) 
        ps = 0;//RST硬件复位默认值
    writeByteCmd(0x84|ps);
}

/**
 * 控制DDRAM显示的扫描方式
 * 
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   0   0   0   1   AC2 AC1 AC0
 * AC0=0 AC1=0:
 * AC0=0 AC1=1:
 * AC0=1 AC1=0:
 * AC0=1 AC1=1:
 * 
 * AC2=0:
 * AC2=1:
 * 
 * ac取值：0~7
*/
void lcd_set_ram_address_control( uint8_t ac ) {
    if ( ac > 7 )
        ac = 0x01;
    writeByteCmd(0x88|ac);
}

/**
 * 设置帧率
 * 
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   0   1   0   0   0   FR1 FR0
 * frate:0 => 0xA0 => FR1:0 FR0:0  76fps
 * frate:1 => 0xA1 => FR1:0 FR0:1  95fps
 * frate:2 => 0xA2 => FR1:1 FR0:0  132fps
 * frate:3 => 0xA3 => FR1:1 FR0:1  168fps
 * frate取值：0~3
*/
void lcd_set_frame_rate( uint8_t frate ) {
    if ( frate > 3 ) 
        frate = 0x01;//RST硬件复位默认值
    writeByteCmd(0xA0|frate);
}

/**
 * 
 * 
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   0   1   0   0   1   0   AP
 * AP=0：默认显示模式
 * AP=1：打开所有的segments
*/
void lcd_all_pixel_on( uint8_t ap ) {
    if ( ap > 1 ) 
        ap = 0;//RST硬件复位默认值
    writeByteCmd(0xA4|ap);
}

/**
 * 反显模式
 * 
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   0   1   0   0   1   1   INV
 * INV=0：默认显示模式
 * INV=1：反显模式，(即数据1、0互换的效果)
 * inv取值：0~1
*/
void lcd_set_inverse_display( uint8_t inv ) {
    if ( inv > 1 ) 
        inv = 0;//RST硬件复位默认值
    writeByteCmd(0xA6|inv);
}

/**
 * 显示开启
 * 
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   0   1   0   1   1   1   PD
 * PD=0：关闭显示（不是控制背光）
 * PD=1：开启显示
 * pd取值：0~1
*/
void lcd_set_display_enable( uint8_t pd ) {
    if ( pd > 1 ) 
        pd = 0;//RST硬件复位默认值
    writeByteCmd(0xAE|pd);
}

/**
 * 设置扫描方向
 * 
 * /**
 *         SEG191                    SEG0
 *     COM0  -------------------------
 *           |                       |--|      
 *           |                       |  |     
 *           |                       |--|
 *     COM63 -------------------------
 * 
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   1   0   0   0   MY  MX  0
 * scan:0 => MY:0 MX:0  (COM0->COM63)(SEG0->SEG191) 
 * scan:1 => MY:0 MX:1  (COM0->COM63)(SEG191->SEG0)
 * scan:2 => MY:1 MX:0  (COM63->COM0)(SEG0->SEG191)
 * scan:3 => MY:1 MX:1  (COM63->COM0)(SEG191->SEG0)
 * 1.数据显示方向由COM扫描方向决定，低位数据先被扫描
 * 2.屏幕显示区域只在SEG191~SEG64
 * scan取值：0~3
*/
void lcd_set_scan_direction( uint8_t scan ) {
    if ( scan > 3 )
        scan = 0;//RST硬件复位默认值
    writeByteCmd(0xC0|(scan<<1));
}

/**
 * 软件复位
 * 
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   1   1   0   0   0   1   0
*/
void lcd_software_reset( void ) {
    writeByteCmd(0xE2);//Software Reset
}

/**
 * 空操作指令
 * 
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   1   1   0   0   0   1   1
*/
void lcd_nop( void ) {
    writeByteCmd(0xE3);//nop
}

/**
 * 设置偏差
 * 
 * 应该是设置屏幕亮度的意思，测试1/9最亮，1/6最暗
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   1   1   0   1   0   BR1 BR0
 * ratio:0 => BR1:0 BR0:0  偏差1/6
 * ratio:1 => BR1:0 BR0:1  偏差1/7
 * ratio:2 => BR1:1 BR0:0  偏差1/8
 * ratio:3 => BR1:1 BR0:1  偏差1/9
 * ratio取值：0~3
*/
void lcd_set_bias_ratio( uint8_t ratio ) {
    if ( ratio > 3 ) 
        ratio = 3;//RST硬件复位默认值
    writeByteCmd(0xE8|ratio);
}

/**
 * 设置局部显示
 * 
 * D7   D6   D5   D4   D3   D2   D1   D0
 * 1    1    1    1    0    0    0    1
 * 0    0    CEN5 CEN4 CEN3 CEN2 CEN1 CEN0
 * 可设置局部显示范围CEN[5:0] : [0,0x3F]
 * comEnd取值0~64
*/
void lcd_set_com_end( uint8_t comEnd ) {
    writeByteCmd(0xF1);
    if ( comEnd > 0x3F ) 
        comEnd = 0x3F;//RST硬件复位默认值
    writeByteCmd(comEnd);
}

/**
 * 
 * 
 * D7   D6   D5   D4   D3   D2   D1   D0
 * 1    1    1    1    0    0    1    0
 * 0    0    DST5 DST4 DST3 DST2 DST1 DST0
 * DST[0,0xFC]
 * dst取值：0~64
*/
void lcd_set_partial_start_address( uint8_t dst ) {
    writeByteCmd(0xF2);
    if ( dst > 0x3F ) 
        dst = 0;//RST硬件复位默认值
    writeByteCmd(dst);    
}

/**
 * 
 * 
 * D7   D6   D5   D4   D3   D2   D1   D0
 * 1    1    1    1    0    0    1    0
 * 0    0    DEN5 DEN4 DEN3 DEN2 DEN1 DEN0
 * DEN[0,0xFC]
 * den取值：0~64
*/
void lcd_set_partial_end_address( uint8_t den ) {
    writeByteCmd(0xF3);    
    if ( den > 0x3F ) 
        den = 0;//RST硬件复位默认值
    writeByteCmd(den);
}


/**
 * 选择测试命令表
 * 
 * D7  D6  D5  D4  D3  D2  D1  D0
 * 1   1   1   1   1   1   1   1
 * 0   0   0   0   0   0   H1  H0
*/
void lcd_test_control( uint8_t h ) {
    writeByteCmd(0xFF);
    writeByteCmd(h);
}






