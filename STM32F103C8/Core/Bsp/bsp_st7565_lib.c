#include "bsp_st7565.h"

/**
 * Display ON/OFF
 * 
 * 打开或关闭显示器
 * on:1 打开  on:0 关闭
*/
void display_on_off( uint8_t on ) {
    uint8_t data;
    if ( on ) {
        data = 0xAF;
    } else {
        data = 0xAE;
    }
    writeByteCmd(data);
}

/**
 * Display Start Line Set
 * 
 * 指定显示RAM数据的显示起始行地址
 * line取值：0~63
*/
void display_start_line_set( uint8_t line ) {
    uint8_t data;
    if ( line > 0x3F ) {
        line = 0x3F;
    }
    data = 0x40 | line;
    writeByteCmd(data);
}

/**
 * 设置页地址
 * 
 * 指定显示RAM数据时对应的页地址
 * page取值：0~8
*/
void page_address_set( uint8_t page ) {
    uint8_t data;
    if ( page > 8 ) {
        page = 0;
    }
    data = 0xB0 | page;
    writeByteCmd(data);
}

/**
 * 设置列地址
 * 
 * 指定显示RAM数据时对应的列地址
 * col取值：0~131
*/
void column_address_set( uint8_t col ) {
    uint8_t data;
    if ( col > 131 ) {
        col = 0;
    }
    data = 0x10 | (col&0xF0) >> 4;
    writeByteCmd(data);
    data = 0x00 | (col&0x0F);
    writeByteCmd(data);
}

/**
 * 将8位数据写入指定的显示数据RAM地址
*/
void display_data_write( uint8_t data ) {
    writeByteData(data);
}

/**
 * 
*/
void adc_select( uint8_t val ) {
    uint8_t data;
    val = val == 1 ? 1 : 0;
    data = 0xA0 | val;
    writeByteCmd(data);
}

/**
 * 屏幕反显
 * val: 0正显 1反显
*/
void display_normal_reverse( uint8_t val ) {
    uint8_t data;
    val = val == 1 ? 1 : 0;
    data = 0xA6 | val;
    writeByteCmd(data);
}

/**
 * 不管RAM的内容是什么，强制打开所有的显示
 * val: 0默认 1强制
*/
void display_all_points_ON_OFF( uint8_t val ) {
    uint8_t data;
    val = val == 1 ? 1 : 0;
    data = 0xA4 | val;
    writeByteCmd(data);
}

/**
 * 选择电压偏置比
 * val: 0 1
 * 
*/
void lcd_bias_set( uint8_t val ) {
    uint8_t data;
    val = val == 1 ? 1 : 0;
    data = 0xA2 | val;
    writeByteCmd(data);
}

/**
 * 临时修改某个位置的数据时设置
 * 执行End命令后，col地址恢复到之前
*/
void read_modify_write( void ) {
    writeByteCmd(0xE0);
}

/**
 * End，col地址恢复到之前
*/
void read_modify_write_end( void ) {
    writeByteCmd(0xEE);
}

/**
 * 复位
*/
void reset( void ) {
    writeByteCmd(0xE2);
}

/**
 * 选择COM输出终端的扫描方向,详细信息请参见
 * "Common Output Mode Select Circuit."
*/
void common_output_mode_select( uint8_t val ) {
    uint8_t data;
    val = val == 1 ? 1 : 0;
    data = 0xC0 | (val<<3);
    writeByteCmd(data);
}

/**
 * 设置电源电路功能
 * val: 0~7
*/
void power_controller_set( uint8_t val ) {
    uint8_t data;
    if ( val > 7 ) {
        val = 0;
    }
    data = 0x28 | val;
    writeByteCmd(data);
}

/**
 * 设置V5电压调节器内部电阻比率
 * val: 0~7
*/
void v5_voltage_regulator_internal_resistor_ratio_set( uint8_t val ) {
    uint8_t data;
    if ( val > 7 ) {
        val = 0;
    }
    data = 0x20 | val;
    writeByteCmd(data);
}

/**
 * 设置对比度
*/
void the_electronic_volume( uint8_t val ) {
    if ( val > 63 ) {
        val = 0;
    }
    writeByteCmd(81);
    writeByteCmd(val);
}

/**
 * 睡眠模式
 * val：0工作 1睡眠
*/
void sleep_mode( uint8_t val ) {
    uint8_t data;
    val = val == 1 ? 1 : 0;
    data = 0xAC | val;
    writeByteCmd(data);
    writeByteCmd(0x00);
}

/**
 * booster 设置
 * val: 0~2
*/
void the_booster_ratio( uint8_t val ) {
    if ( val > 2 ) {
        val = 0;
    }
    writeByteCmd(0xF8);
    writeByteCmd(val);
}

/**
 * 空指令
*/
void nop_( void ) {
    writeByteCmd(0xE3);
}

/**
 * 测试指令
*/
void test_( void ) {
    writeByteCmd(0xFC);
}


