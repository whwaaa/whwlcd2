#include "bsp_st7571.h"

/**
 * 帧freq: 0~15
 * 升压booster: 0~3
*/
void st7571_set_mode( uint8_t freq, uint8_t booster ) {
	st7571_writeByteCmd( 0x38 );
	st7571_writeByteCmd( (freq<<4) | (booster<<2) );
}

/**
 * 写数据
*/
void st7571_write_display_data( uint8_t data ) {
	st7571_writeByteData( data );
}

/**
 * 开启关闭显示图标功能，图标页固定16，默认关闭该功能设置页16无效
 * sw: 0关闭, 1开启
 */
void st7571_set_icon( uint8_t sw ) {
	uint8_t val;
	val = sw > 1 ? 0 : sw;
	st7571_writeByteCmd( 0xA2 | val );
}

/**
 * 设置页地址
 * 注意，开启图标显示功能后页地址才能设置为16
 * addr: 0~15
*/
void st7571_set_page_address( uint8_t addr ) {
	uint8_t val;
	val = addr > 0x0F ? 0 : addr;
	st7571_writeByteCmd( 0xB0 | val );
}

/**
 * 设置列地址
 * 在写入数据时，列地址会随写入的数据而自增
 * col: 0~127
*/
void st7571_set_column_address( uint8_t col ) {
	uint8_t val;
	val = col > 127 ? 0 : col;
	//set (MSB)
	st7571_writeByteCmd( 0x10 | (val>>4) );
	//set (LSB)
	st7571_writeByteCmd( 0x00 | (val&0x0F) );
}

/**
 * 显示开关
 * sw: 0关 1开
*/
void st7571_display_on_off( uint8_t sw ) {
	uint8_t val;
	val = sw > 1 ? 0 : sw;
	st7571_writeByteCmd( 0xAE | val );
}

/**
 * 设置顶部开始显示的行
 * line: 0~127
*/
void st7571_set_display_start_line( uint8_t line ) {
	uint8_t val;
	val = line > 127 ? 0 : line;
	st7571_writeByteCmd( 0x40 );
	st7571_writeByteCmd( val );
}

/**
 * 设置LCD开始显示的列数据
 * 通过这个指令可以在不改变数据的情况下，移动视窗
 * com: 0~127
*/
void st7571_set_com0( uint8_t com ) {
	uint8_t val;
	val = com > 127 ? 0 : com;
	st7571_writeByteCmd( 0x44 );
	st7571_writeByteCmd( val );
}

/**
 * 设置这什么好像是占空比？包括图标也生效
 * duty: 0~127
*/
void st7571_set_display_duty( uint8_t duty ) {
	uint8_t val;
	val = duty > 127 ? 0 : duty;
	st7571_writeByteCmd( 0x48 );
	st7571_writeByteCmd( val );
}

/**
 * 控制内部帧信号的相位来提高显示质量
 * 没研究,手册建议是填0x0A
 * inv: 0~31
*/
void st7571_set_n_line_inversion( uint8_t inv ) {
	uint8_t val;
	val = inv > 31 ? 0x0A : inv;
	st7571_writeByteCmd( 0x4C );
	st7571_writeByteCmd( val );
	
}

/**
 * 该指令将反转模式从n线反转转换为帧反转。
 * 不懂
*/
void st7571_release_n_line_inversion( void ) {
	st7571_writeByteCmd( 0xE4 );
}

/**
 * 反显
 * redis: 0正常显示 1反显数据
*/
void st7571_reverse_display( uint8_t redis ) {
	uint8_t val;
	val = redis > 1 ? 0 : redis;
	st7571_writeByteCmd( 0xA6 | val );
}

/**
 * 该指令会强制打开整个LCD像素
 * 该指令优先于反显指令（我理解打开这个指令时反显指令是无效的）
 * ent: 0关闭 1强制打开整个LCD像素
*/
void st7571_entire_display_on( uint8_t ent ) {
	uint8_t val;
	val = ent > 1 ? 0 : ent;
	st7571_writeByteCmd( 0xA4 | val );
}

/**
 * 电源设置，不懂，看例程能点亮就行
 * power: 0~7
*/
void st7571_power_control( uint8_t power ) {
	uint8_t val;
	val = power > 7 ? 0 : power;
	st7571_writeByteCmd( 0x28 | val );
}

/**
 * 选择电阻调节器，不懂，调试出接受的亮度就行
 * regu: 0~7
*/
void st7571_select_regulator_register( uint8_t regu ) {
	uint8_t val;
	val = regu > 7 ? 0 : regu;
	st7571_writeByteCmd( 0x20 | val );
}

/**
 * 基准电压，不懂，调试出接受的亮度就行
 * ev: 0~127
*/
void st7571_set_contrast( uint8_t ev ) {
	uint8_t val;
	val = ev > 127 ? 0 : ev;
	st7571_writeByteCmd( 0x81 );
	st7571_writeByteCmd( val );
}

/**
 * 不懂，调试出接受的亮度就行
 * bias: 0~7
*/
void st7571_select_lcd_bias( uint8_t bias ) {
	uint8_t val;
	val = bias > 7 ? 0 : bias;
	st7571_writeByteCmd( 0x50 | val );
}

/**
 * 设置LCD COM扫描方向
 * SHL = 0: normal direction (COM0 ~ COM127)
 * SHL = 1: reverse direction (COM127 ~ COM0)
 * shl: 0默认 1反方向
*/
void st7571_set_com_scan_direction( uint8_t shl ) {
	uint8_t val;
	val = shl > 1 ? 0 : shl;
	st7571_writeByteCmd( 0xC0 | (val<<4) );
}

/**
 * 设置LCD SEG扫描方向
 * ADC = 0: normal direction (SEG0 ~ SEG127) 
 * ADC = 1: reverse direction (SEG127 ~ SEG0)
 * adc: 0默认 1反方向
*/
void st7571_set_seg_scan_direction( uint8_t adc ) {
	uint8_t val;
	val = adc > 1 ? 0 : adc;
	st7571_writeByteCmd( 0xA0 | val );
}

/**
 * 启用内置振荡器电路，不知道有什么用
*/
void st7571_oscillator_on( void ) {
	st7571_writeByteCmd( 0xAB );
}

/**
 * 低功耗省电模式
 * 开启之后屏幕清空像素不显示
 * p = 0: normal mode 
 * p = 1: power-save mode (sleep mode)
*/
void st7571_set_power_save_mode( uint8_t p ) {
	uint8_t val;
	val = p > 1 ? 0 : p;
	st7571_writeByteCmd( 0xA8 | val );
}

/**
 * 退出省电模式
*/
void st7571_release_power_save_mode( void ) {
	st7571_writeByteCmd( 0xE1 );
}

/**
 * 软件复位
*/
void st7571_reset( void ) {
	st7571_writeByteCmd( 0xE2 );
}

/**
 * 3线spi模式下不使用A0，使用这条指令表示要显示的数据字节数，
 * 传输完指定字节数的数据后，下一个数据做为指令。（但是DC端口好像也可以做为数据命令选择端口？）
 * len: 0~255
*/
void st7571_set_display_data_length( uint8_t len ) {
	st7571_writeByteCmd( 0xE8 );
	st7571_writeByteCmd( len );
}

/**
 * 空指令
*/
void st7571_nop( void ) {
	st7571_writeByteCmd( 0xE3 );
}

/**
 * 此指令启用扩展命令集1。
 */
void st7571_extension_command_set1( void ) {
	st7571_writeByteCmd( 0xFD );
}

/**
 * 此指令启用扩展命令集2。
 */
void st7571_extension_command_set2( void ) {
	st7571_writeByteCmd( 0xD1 );
}

/**
 * 此指令启用扩展命令集3。
 */
void st7571_extension_command_set3( void ) {
	st7571_writeByteCmd( 0x7B );
}

/*-------------------- EXTENSION COMMAND SET 1 ----------------------*/
void st7571_ex1_increase_vop_offset( void ) {
	st7571_writeByteCmd( 0x51 );
}

void st7571_ex1_decrease_vop_offset( void ) {
	st7571_writeByteCmd( 0x52 );
}

/**
 * 退出扩展命令1，回到默认模式
*/
void st7571_ex1_return_normal_mode( void ) {
	st7571_writeByteCmd( 0x00 );
}
/*-------------------------------------------------------------------*/



/*-------------------- EXTENSION COMMAND SET 2 ----------------------*/
/**
 * 该指令禁用EEPROM自动读取功能，并允许手动设置相关寄存器。不懂
*/
void st7571_ex2_disable_autoread( void ) {
	st7571_writeByteCmd( 0xAA );
}

/**
 * 该指令进入EEPROM模式。不懂
*/
void st7571_ex2_enter_eeprom_mode( void ) {
	st7571_writeByteCmd( 0x13 );
}

/**
 * 不懂
*/
void st7571_ex2_enable_read_mode( void ) {
	st7571_writeByteCmd( 0x20 );
}

/**
 * 不懂
*/
void st7571_ex2_set_read_pulse( void ) {
	st7571_writeByteCmd( 0x71 );
}

/**
 * 不懂
*/
void st7571_ex2_exit_eeprom_mode( void ) {
	st7571_writeByteCmd( 0x83 );
}

/**
 * 不懂
*/
void st7571_ex2_enable_erase_mode( void ) {
	st7571_writeByteCmd( 0x4A );
}

/**
 * 不懂
*/
void st7571_ex2_set_erase_pulse( void ) {
	st7571_writeByteCmd( 0x55 );
}

/**
 * 不懂
*/
void st7571_ex2_enable_write_mode( void ) {
	st7571_writeByteCmd( 0x35 );
}

/**
 * 不懂
*/
void st7571_ex2_set_write_pulse( void ) {
	st7571_writeByteCmd( 0x6A );
}

/**
 * 退出扩展指令2，回到正常模式
*/
void st7571_ex2_return_normal_mode( void ) {
	st7571_writeByteCmd( 0x00 );
}
/*-------------------------------------------------------------------*/


/*-------------------- EXTENSION COMMAND SET 3 ----------------------*/
/**
 * 该指令控制灰度模式
 * B/G=0: IC处于灰度模式(为8像素写入2字节)。
 * B/G=1: IC为黑白模式(8像素写入1字节)。
 * bg: 0灰度 1黑白
 * 4阶灰度的理解:
 * 数据为2字节,第一字节可理解"位",第二字节可理解"灰度"
 * 有四种亮度: 
 * 1.亮度++++[位bit1灰度bit1]
 * 2.亮度+++ [位bit1灰度bit0]
 * 3.亮度++  [位bit0灰度bit1]
 * 4.亮度+   [位bit0灰度bit0]
 * 例如: 
 * 在一列8像素显示最亮的数据传输: 0xFF 0xFF
 * 在一列8像素显示次亮的数据传输: 0xFF 0x00
 * 在一列8像素显示次暗的数据传输: 0x00 0xFF
 * 在一列8像素显示最暗的数据传输: 0x00 0x00
*/
void st7571_ex3_set_color_mode( uint8_t gb ) {
	uint8_t val;
	val = gb > 1 ? 0 : gb;
	st7571_writeByteCmd( 0x10 | val );
	st7571_ex3_return_normal_mode();
}

void st7571_ex3_return_normal_mode( void ) {
	st7571_writeByteCmd( 0x00 );
}
/*-------------------------------------------------------------------*/



