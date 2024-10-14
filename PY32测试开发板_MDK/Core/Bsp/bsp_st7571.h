#ifndef __BSP_ST7571__H
#define __BSP_ST7571__H

#include "common.h"

#define PAGE_HOME                   0
#define PAGE_MENU                   1
#define PAGE_PAINTING               2
#define PAGE_TOUCH_CALIBRATION      3
#define PAGE_IMG                    4
typedef struct _PAGE {
    uint8_t page;
    uint8_t ram[10][128];
} PAGE_STR;
extern PAGE_STR page_str;

/*----------------------- SPI接口 ----------------------*/
void st7571_writeByteCmd( uint8_t data );
void st7571_writeByteData( uint8_t data );
/*----------------------- SPI接口end ----------------------*/

/*----------------------ST7575驱动相关----------------------*/
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
/*----------------------ST7575驱动相关 end----------------------------*/


void writeFont_20x20( uint8_t x, uint8_t y, char *font );
void writeFont_16x16( uint8_t x, uint8_t y, char *font );
void writeFont_24x24( uint8_t x, uint8_t y, char *font );
void writeLogo_0( uint8_t x, uint8_t y );
void writeFont_ASCII8x16( uint8_t x, uint8_t y, char *data, uint8_t isBold );
void st7571_lcd_clear( void );
void st7571_writeDataToRAM( uint8_t page, uint8_t startX, uint8_t endX, uint8_t *pdata );
void st7571_lcd_init( void );

void st7571_lcd_display_home( void );
void st7571_lcd_display_menu( void );
void st7571_lcd_display_painting( void );
void st7571_lcd_display_painting_clear( void );
void st7571_lcd_display_touch_calibration( void );
void st7571_lcd_display_img( void );
void st7571_painting( uint8_t x, uint8_t y );



#endif /*__BSP_ST7571__H*/



