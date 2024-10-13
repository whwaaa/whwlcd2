#ifndef __BSP_ST7571__H
#define __BSP_ST7571__H

#include "common.h"

//#define COLOR_MODE  1//黑白点阵
#define COLOR_MODE  0//4阶灰度

/*----------------------- 触摸相关start ----------------------*/
//检测X坐标
#define TOUCH_X_PWR_ON \
    HAL_GPIO_WritePin(X_GPIO_Port, X_Pin, GPIO_PIN_RESET);\
    HAL_GPIO_WritePin(X__GPIO_Port, X__Pin, GPIO_PIN_SET);
//断开检测x电源
#define TOUCH_X_PWR_OFF \
    HAL_GPIO_WritePin(X_GPIO_Port, X_Pin, GPIO_PIN_SET);\
    HAL_GPIO_WritePin(X__GPIO_Port, X__Pin, GPIO_PIN_RESET);
//检测y坐标
#define TOUCH_Y_PWR_ON \
    HAL_GPIO_WritePin(Y_GPIO_Port, Y_Pin, GPIO_PIN_RESET);\
    HAL_GPIO_WritePin(Y__GPIO_Port, Y__Pin, GPIO_PIN_SET);
//断开检测y电源
#define TOUCH_Y_PWR_OFF \
    HAL_GPIO_WritePin(Y_GPIO_Port, Y_Pin, GPIO_PIN_SET);\
    HAL_GPIO_WritePin(Y__GPIO_Port, Y__Pin, GPIO_PIN_RESET);
//Y+接VCC，X-接GND
#define TOUCH_F_PWR_ON \
    HAL_GPIO_WritePin(Y_GPIO_Port, Y_Pin, GPIO_PIN_RESET);\
    HAL_GPIO_WritePin(X__GPIO_Port, X__Pin, GPIO_PIN_SET);  
//断开检测触摸力度电源
#define TOUCH_F_PWR_OFF \
    HAL_GPIO_WritePin(Y_GPIO_Port, Y_Pin, GPIO_PIN_SET);\
    HAL_GPIO_WritePin(X__GPIO_Port, X__Pin, GPIO_PIN_RESET);
//断开所有
#define TOUCH_ALL_PWR_OFF \
    TOUCH_X_PWR_OFF\
    TOUCH_Y_PWR_OFF
/*----------------------- 触摸相关end ----------------------*/

void st7571_writeByteCmd( uint8_t data );
void st7571_writeByteData( uint8_t data );

void st7571_set_mode( uint8_t freq, uint8_t booster );
void st7571_write_display_data( uint8_t data );
void st7571_set_icon( uint8_t sw );
void st7571_set_page_address( uint8_t addr );
void st7571_set_column_address( uint8_t col );
void st7571_display_on_off( uint8_t sw );
void st7571_set_display_start_line( uint8_t line );
void st7571_set_com0( uint8_t com );
void st7571_set_display_duty( uint8_t duty );
void st7571_set_n_line_inversion( uint8_t inv );
void st7571_release_n_line_inversion( void );
void st7571_reverse_display( uint8_t redis );
void st7571_entire_display_on( uint8_t ent );
void st7571_power_control( uint8_t power );
void st7571_select_regulator_register( uint8_t regu );
void st7571_set_contrast( uint8_t ev );
void st7571_select_lcd_bias( uint8_t bias );
void st7571_set_com_scan_direction( uint8_t shl );
void st7571_set_seg_scan_direction( uint8_t adc );
void st7571_oscillator_on( void );
void st7571_set_power_save_mode( uint8_t p );
void st7571_release_power_save_mode( void );
void st7571_reset( void );
void st7571_set_display_data_length( uint8_t len );
void st7571_nop( void );
void st7571_extension_command_set1( void );
void st7571_extension_command_set2( void );
void st7571_extension_command_set3( void );

/*-------------------- EXTENSION COMMAND SET 1 ----------------------*/
void st7571_ex1_increase_vop_offset( void );
void st7571_ex1_decrease_vop_offset( void );
void st7571_ex1_return_normal_mode( void );
/*-------------------------------------------------------------------*/

/*-------------------- EXTENSION COMMAND SET 2 ----------------------*/
void st7571_ex2_disable_autoread( void );
void st7571_ex2_enter_eeprom_mode( void );
void st7571_ex2_enable_read_mode( void );
void st7571_ex2_set_read_pulse( void );
void st7571_ex2_exit_eeprom_mode( void );
void st7571_ex2_enable_erase_mode( void );
void st7571_ex2_set_erase_pulse( void );
void st7571_ex2_enable_write_mode( void );
void st7571_ex2_set_write_pulse( void );
void st7571_ex2_return_normal_mode( void );
/*-------------------------------------------------------------------*/

/*-------------------- EXTENSION COMMAND SET 3 ----------------------*/
void st7571_ex3_set_color_mode( uint8_t gb );
void st7571_ex3_return_normal_mode( void );
/*-------------------------------------------------------------------*/

void writeFont_20x20( uint8_t x, uint8_t y, char *font );
void writeFont_16x16( uint8_t x, uint8_t y, char *font );
void writeFont_24x24( uint8_t x, uint8_t y, char *font );
void writeLogo_0( uint8_t x, uint8_t y );
void writeFont_ASCII8x16( uint8_t x, uint8_t y, char *data, uint8_t isBold );
void st7571_lcd_clear( void );
void st7571_writeDataToRAM( uint8_t page, uint8_t startX, uint8_t endX, uint8_t *pdata );
void st7571_lcd_init( void );
void st7571_lcd_test_display( void );

uint32_t touch_check_x( void );
uint32_t touch_check_y( void );
void touch_calibration( void );

#endif /*__BSP_ST7571__H*/



