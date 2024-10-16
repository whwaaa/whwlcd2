#include "bsp_st7571.h"

PAGE_STR page_str = {0};


/**
 * 清屏
 * 分15个页，每页数据8bit*128
 */
void st7571_lcd_clear( void ) {
    for ( uint8_t page=0; page<=15; page++ ) {
        //设置页地址
        st7571_set_page_address(page);
        //设置列地址
        st7571_set_column_address(0);
        //写入数据
        for ( int i=0; i<128; i++ ) {
            #if COLOR_MODE
            st7571_write_display_data(0x00);
            #else
            st7571_write_display_data(0x00);
            st7571_write_display_data(0x00);
            #endif
        }
    }
}
/**
 * 指定位置写入数据
 * page：取值0~15共16页数据页,第16页仅开启图标才能使用
 * startX：起始x坐标，取值0~127
 * endX：最后x坐标，必须大于startX，取值0~127
 * pdata：数据长度必须大于等于(startX-endX+1)
 */
void st7571_writeDataToRAM( uint8_t page, uint8_t startX, uint8_t endX, uint8_t *pdata, uint8_t isImg ) {
    uint8_t xlen;
    if ( startX>endX || endX==0 || endX>127 ) return;
    if ( page > 0x0F ) return;
    xlen = endX - startX + 1;
    //设置页地址
    st7571_set_page_address( page );
    //设置列地址
    st7571_set_column_address(startX);
    //写入数据
    for ( uint8_t i=0; i<xlen; i++ ) {
        #if COLOR_MODE
        st7571_write_display_data(pdata[i]);
        #else
        if ( isImg ) {
            st7571_write_display_data(pdata[i*2]);
            st7571_write_display_data(pdata[i*2+1]);
        } else {
            st7571_write_display_data(pdata[i]);
            st7571_write_display_data(pdata[i]);
        }
        #endif
    }
}
/**
 * LCD初始化
 */ 
void st7571_lcd_init( void ) {
    //硬件复位
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    
    st7571_reset();//软件复位
    HAL_Delay(10);
    st7571_display_on_off(0);//display off
    st7571_set_mode(0x0B,0x02);//FR=1011=>85HZ, BE=1,0=>booster efficiency Level-3
    st7571_set_com_scan_direction(1);//行扫描方向(不明白,0/1设置看着没什么区别)
    st7571_set_seg_scan_direction(1);//列扫描方向
    st7571_set_display_start_line(96);//设置开始显示的数据行(设置96刚好对应从0页写入数据时从最顶上显示)
    st7571_set_com0(0);//设置开始显示的数据列
    st7571_oscillator_on();//OSC. ON
    st7571_select_lcd_bias(0x06);//Set LCD Bias=1/9 V0
    st7571_select_regulator_register(0x06);//Select regulator register(1+(Ra+Rb))
    st7571_set_contrast(0x7e);//设置基准电压  EV=35 => Vop =10.556V
    // st7571_writeByteCmd( 0xF3 );//DC-DC step up, 8 times boosting circuit 
    // st7571_writeByteCmd( 0x67 );
    // st7571_writeByteCmd( 0x04 );
    // st7571_writeByteCmd( 0x93 );
    st7571_power_control(0x04); //Power Control, VC: ON VR: OFF VF: OFF
    HAL_Delay(200);
    st7571_power_control(0x06); //Power Control, VC: ON VR: ON VF: OFF
    HAL_Delay(200);
    st7571_power_control(0x07); //Power Control, VC: ON VR: ON VF: ON
    HAL_Delay(10);
    st7571_extension_command_set3();//进入扩展指令3
#if COLOR_MODE
    st7571_ex3_set_color_mode(1);//0四阶灰度模式，1黑白模式，并退出扩展指令恢复正常模式
#else
    st7571_ex3_set_color_mode(0);//0四阶灰度模式，1黑白模式，并退出扩展指令恢复正常模式    
#endif
    st7571_display_on_off(1);//display on
    HAL_Delay(10);
    // st7571_entire_display_on(1);//强制开启LCD所有像素
    st7571_lcd_clear();
    // st7571_set_power_save_mode(1);//进入省电模式（省电模式屏幕会关闭，数据会隐藏）
    // st7571_release_power_save_mode();//退出省电模式
}
/**
 * SPI写命令接口
 */
void st7571_writeByteCmd( uint8_t cmd ) {
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);//0命令
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);//CS_L
    HAL_SPI_Transmit( &hspi1, &cmd, 1, 1000 );
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);//CS_H
}
/**
 * SPI写数据接口
 */
void st7571_writeByteData( uint8_t data ) {
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);//1数据
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);//CS_L
    HAL_SPI_Transmit( &hspi1, &data, 1, 1000 );
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);//CS_H
}
/**
 * 任意位置显示24x24字符
 * x取值：0~127
 * y取值：0~96
 * font：显示的数据
*/
void writeFont_24x24( uint8_t x, uint8_t y, char *font ) {
    uint8_t page, fontLen, yu, pNum;
    uint16_t xlen, xlen_t;

    xlen = strlen(font) / 3 * 24;//utf8一个字3字节
    xlen = xlen > (128 - x) ? (128 - x) : xlen;
    fontLen = (xlen+23)/24;//字数长度
    //计算RAM页地址(0~3)
    page = y / 8;
    yu = y % 8;
    if ( yu == 0 ) {
        pNum = 3;//一个字高24占3页
    } else {
        pNum = 4;//一个字高24占4页
    }

    for ( uint8_t i=0; i<pNum&&page<12; i++,page++ ) {
        xlen_t = xlen;
        //设置页地址
        st7571_set_page_address(page);
        //设置列地址
        st7571_set_column_address(x);

        if ( yu == 0 ) {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(myFontIdx_24)/3); idx++ ) {
                    if ( (uint32_t)(font[b*3]<<16|font[b*3+1]<<8|font[b*3+2]) == myFontIdx_24[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<24&&xlen_t>0; j++ ) {
                            xlen_t--;
                            #if COLOR_MODE
                            st7571_write_display_data(myFont_24[idx*3+i][j]);
                            #else
                            st7571_write_display_data(myFont_24[idx*3+i][j]);
                            st7571_write_display_data(myFont_24[idx*3+i][j]);
                            //st7571_write_display_data(0x00);
                            #endif
                        }
                        break;
                    }
                }
            }
        } else {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(myFontIdx_24)/3); idx++ ) {
                    if ( (uint32_t)(font[b*3]<<16|font[b*3+1]<<8|font[b*3+2]) == myFontIdx_24[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<24&&xlen_t>0; j++ ) {
                            xlen_t--;
                            if ( i == 0 ) {
                                #if COLOR_MODE
                                st7571_write_display_data(myFont_24[idx*3+i][j]<<yu);
                                #else
                                st7571_write_display_data(myFont_24[idx*3+i][j]<<yu);
                                st7571_write_display_data(myFont_24[idx*3+i][j]<<yu);
                                #endif
                            } else if ( i == 1 ) {
                                #if COLOR_MODE
                                st7571_write_display_data((myFont_24[idx*3+(i-1)][j]>>(8-yu))|(myFont_24[idx*3+i][j]<<yu));
                                #else
                                st7571_write_display_data((myFont_24[idx*3+(i-1)][j]>>(8-yu))|(myFont_24[idx*3+i][j]<<yu));
                                st7571_write_display_data((myFont_24[idx*3+(i-1)][j]>>(8-yu))|(myFont_24[idx*3+i][j]<<yu));
                                #endif
                            } else if ( i == 2 ) {
                                #if COLOR_MODE
                                st7571_write_display_data((myFont_24[idx*3+(i-1)][j]>>(8-yu))|(myFont_24[idx*3+i][j]<<yu));
                                #else
                                st7571_write_display_data((myFont_24[idx*3+(i-1)][j]>>(8-yu))|(myFont_24[idx*3+i][j]<<yu));
                                st7571_write_display_data((myFont_24[idx*3+(i-1)][j]>>(8-yu))|(myFont_24[idx*3+i][j]<<yu));
                                #endif
                            } else if ( i == 3 ) {
                                #if COLOR_MODE
                                st7571_write_display_data((myFont_24[idx*3+(i-1)][j]>>(8-yu)));
                                #else
                                st7571_write_display_data((myFont_24[idx*3+(i-1)][j]>>(8-yu)));
                                st7571_write_display_data((myFont_24[idx*3+(i-1)][j]>>(8-yu)));
                                #endif
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}
/**
 * 任意位置显示20x20字符
 * x取值：0~127
 * y取值：0~96
 * font：显示的数据
*/
void writeFont_20x20( uint8_t x, uint8_t y, char *font ) {
    uint8_t page, fontLen, yu, pNum;
    uint16_t xlen, xlen_t;

    xlen = strlen(font) / 3 * 20;//utf8一个字3字节
    xlen = xlen > (128 - x) ? (128 - x) : xlen;
    fontLen = (xlen+19)/20;//字数长度
    //计算RAM页地址(0~3)
    page = y / 8;
    yu = y % 8;
    if ( yu == 0 ) {
        pNum = 3;//一个字高20占3页
    } else {
        pNum = 4;//一个字高20占4页
    }

    for ( uint8_t i=0; i<pNum&&page<12; i++,page++ ) {
        xlen_t = xlen;
        //设置页地址
        st7571_set_page_address(page);
        //设置列地址
        st7571_set_column_address(x);

        if ( yu == 0 ) {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(myFontIdx_20)/3); idx++ ) {
                    if ( (uint32_t)(font[b*3]<<16|font[b*3+1]<<8|font[b*3+2]) == myFontIdx_20[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<20&&xlen_t>0; j++ ) {
                            xlen_t--;
                            #if COLOR_MODE
                            st7571_write_display_data(myFont_20_bold[idx*3+i][j]);
                            #else
                            st7571_write_display_data(myFont_20_bold[idx*3+i][j]);
                            st7571_write_display_data(myFont_20_bold[idx*3+i][j]);
                            #endif
                        }
                        break;
                    }
                }
            }
        } else {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(myFontIdx_20)/3); idx++ ) {
                    if ( (uint32_t)(font[b*3]<<16|font[b*3+1]<<8|font[b*3+2]) == myFontIdx_20[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<20&&xlen_t>0; j++ ) {
                            xlen_t--;
                            if ( i == 0 ) {
                                #if COLOR_MODE
                                st7571_write_display_data(myFont_20_bold[idx*3+i][j]<<yu);
                                #else
                                st7571_write_display_data(myFont_20_bold[idx*3+i][j]<<yu);
                                st7571_write_display_data(myFont_20_bold[idx*3+i][j]<<yu);
                                #endif
                            } else if ( i == 1 ) {
                                #if COLOR_MODE
                                st7571_write_display_data((myFont_20_bold[idx*3+(i-1)][j]>>(8-yu))|(myFont_20_bold[idx*3+i][j]<<yu));
                                #else
                                st7571_write_display_data((myFont_20_bold[idx*3+(i-1)][j]>>(8-yu))|(myFont_20_bold[idx*3+i][j]<<yu));
                                st7571_write_display_data((myFont_20_bold[idx*3+(i-1)][j]>>(8-yu))|(myFont_20_bold[idx*3+i][j]<<yu));
                                #endif
                            } else if ( i == 2 ) {
                                #if COLOR_MODE
                                st7571_write_display_data((myFont_20_bold[idx*3+(i-1)][j]>>(8-yu))|(myFont_20_bold[idx*3+i][j]<<yu));
                                #else
                                st7571_write_display_data((myFont_20_bold[idx*3+(i-1)][j]>>(8-yu))|(myFont_20_bold[idx*3+i][j]<<yu));
                                st7571_write_display_data((myFont_20_bold[idx*3+(i-1)][j]>>(8-yu))|(myFont_20_bold[idx*3+i][j]<<yu));
                                #endif
                            } else if ( i == 3 ) {
                                #if COLOR_MODE
                                st7571_write_display_data((myFont_20_bold[idx*3+(i-1)][j]>>(8-yu)));
                                #else
                                st7571_write_display_data((myFont_20_bold[idx*3+(i-1)][j]>>(8-yu)));
                                st7571_write_display_data((myFont_20_bold[idx*3+(i-1)][j]>>(8-yu)));
                                #endif
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}
/**
 * 任意位置显示16x16字符
 * x取值：0~127
 * y取值：0~96
 * font：显示的数据
*/
void writeFont_16x16( uint8_t x, uint8_t y, char *font ) { 
    uint8_t page, fontLen, yu, pNum;
    uint16_t xlen, xlen_t;

    xlen = strlen(font) / 3 * 16;//utf8一个字3字节
    xlen = xlen > (128 - x) ? (128 - x) : xlen;
    fontLen = (xlen+15)/16;//字数长度
    //计算RAM页地址(0~3)
    page = y / 8;
    yu = y % 8;
    if ( yu == 0 ) {
        pNum = 2;//一个字高16占2页
    } else {
        pNum = 3;//一个字高16占3页
    }
    
    for ( uint8_t i=0; i<pNum&&page<12; i++,page++ ) {
        xlen_t = xlen;
        //设置页地址
        st7571_set_page_address(page);
        //设置列地址
        st7571_set_column_address(x);

        if ( yu == 0 ) {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(myFontIdx_16)/3); idx++ ) {
                    if ( (uint32_t)(font[b*3]<<16|font[b*3+1]<<8|font[b*3+2]) == myFontIdx_16[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<16&&xlen_t>0; j++ ) {
                            xlen_t--;
                            #if COLOR_MODE
                            st7571_write_display_data(myFont_16[idx*2+i][j]);
                            #else
                            st7571_write_display_data(myFont_16[idx*2+i][j]);
                            st7571_write_display_data(myFont_16[idx*2+i][j]);
                            #endif
                        }
                        break;
                    }
                }
            }
        } else {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(myFontIdx_16)/3); idx++ ) {
                    if ( (uint32_t)(font[b*3]<<16|font[b*3+1]<<8|font[b*3+2]) == myFontIdx_16[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<16&&xlen_t>0; j++ ) {
                            xlen_t--;
                            if ( i == 0 ) {
                                #if COLOR_MODE
                                st7571_write_display_data(myFont_16[idx*2+i][j]<<yu);
                                #else
                                st7571_write_display_data(myFont_16[idx*2+i][j]<<yu);
                                st7571_write_display_data(myFont_16[idx*2+i][j]<<yu);
                                #endif
                            } else if ( i == 1 ) {
                                #if COLOR_MODE
                                st7571_write_display_data((myFont_16[idx*2+(i-1)][j]>>(8-yu))|(myFont_16[idx*2+i][j]<<yu));
                                #else
                                st7571_write_display_data((myFont_16[idx*2+(i-1)][j]>>(8-yu))|(myFont_16[idx*2+i][j]<<yu));
                                st7571_write_display_data((myFont_16[idx*2+(i-1)][j]>>(8-yu))|(myFont_16[idx*2+i][j]<<yu));
                                #endif
                            } else if ( i == 2 ) {
                                #if COLOR_MODE
                                st7571_write_display_data((myFont_16[idx*2+(i-1)][j]>>(8-yu)));
                                #else
                                st7571_write_display_data((myFont_16[idx*2+(i-1)][j]>>(8-yu)));
                                st7571_write_display_data((myFont_16[idx*2+(i-1)][j]>>(8-yu)));
                                #endif
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}
/**
 * 任意位置显示8x16大小ASCII字符
 * x取值：0~127
 * y取值：0~96
 * data：显示的数据
 * isBold：1粗体，0非粗体
*/
void writeFont_ASCII8x16( uint8_t x, uint8_t y, char *data, uint8_t isBold ) {  
    uint8_t page, fontLen, yu, xlen_t, (*asciiFont_t)[16], pNum;
    uint16_t xlen;

    xlen = strlen(data) * 8;//utf8一个字3字节
    xlen = xlen > (128 - x) ? (128 - x) : xlen;
    fontLen = strlen(data);//字数长度
    //计算RAM页地址(0~8)
    page = y / 8;
    yu = y % 8;
    if ( isBold ) {
        //asciiFont_t = (uint8_t(*)[16])asciiFont_bold;
    } else {
        //asciiFont_t = (uint8_t(*)[16])asciiFont;
    }
    if ( yu == 0 ) {
        pNum = 2;//一个字高16占2页
    } else {
        pNum = 3;//一个字高16占3页
    }

    for ( uint8_t i=0; i<pNum&&page<12; i++,page++ ) {
        xlen_t = xlen;
        //设置页地址
        st7571_set_page_address(page);
        //设置列地址
        st7571_set_column_address(x);
        if ( yu == 0 ) {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(asciiFontIdx)); idx++ ) {
                     if ( data[b] == asciiFontIdx[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<8&&xlen_t>0; j++ ) {
                            xlen_t--;                            
                            #if COLOR_MODE
                            st7571_write_display_data(asciiFont_t[idx][i*8+j]);
                            #else
                            st7571_write_display_data(asciiFont_t[idx][i*8+j]);
                            st7571_write_display_data(asciiFont_t[idx][i*8+j]);
                            #endif
                        }
                        break;
                    }
                }
            }
        } else {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(asciiFontIdx)); idx++ ) {
                     if ( data[b] == asciiFontIdx[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<8&&xlen_t>0; j++ ) {
                            xlen_t--;
                            if ( i == 0 ) {
                                #if COLOR_MODE
                                st7571_write_display_data(asciiFont_t[idx][i*8+j]<<yu);
                                #else
                                st7571_write_display_data(asciiFont_t[idx][i*8+j]<<yu);
                                st7571_write_display_data(asciiFont_t[idx][i*8+j]<<yu);
                                #endif
                            } else if ( i == 1 ) {
                                #if COLOR_MODE
                                st7571_write_display_data(asciiFont_t[idx][j]>>(8-yu)|asciiFont_t[idx][i*8+j]<<yu);
                                #else
                                st7571_write_display_data(asciiFont_t[idx][j]>>(8-yu)|asciiFont_t[idx][i*8+j]<<yu);
                                st7571_write_display_data(asciiFont_t[idx][j]>>(8-yu)|asciiFont_t[idx][i*8+j]<<yu);
                                #endif
                            } else if ( i == 2 ) {
                                #if COLOR_MODE
                                st7571_write_display_data(asciiFont_t[idx][i*8+j]>>(8-yu));
                                #else
                                st7571_write_display_data(asciiFont_t[idx][8+j]>>(8-yu));
                                st7571_write_display_data(asciiFont_t[idx][8+j]>>(8-yu));
                                #endif
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}
/**
 * 任意位置显示logo
 * x取值：0~127
 * y取值：0~96(8的整数倍)
 * data：显示的数据
*/
void writeLogo_0( uint8_t x, uint8_t y ) {
    uint8_t page, xlen, xlen_t, yu;

    xlen = 128 - x;
    page = y / 8;
    yu = y % 8;

    for ( uint8_t i=0; i<4&&page<12; i++,page++ ) {
        xlen_t = xlen;
        //设置页地址
        st7571_set_page_address(page);
        //设置列地址
        st7571_set_column_address(x);
        if ( yu == 0 ) {
            for ( uint8_t j=0; j<32&&xlen_t>0; j++ ) {
                xlen_t--;
                #if COLOR_MODE
                st7571_write_display_data(logo_0[i][j]);
                #else
                st7571_write_display_data(logo_0[i][j]);
                st7571_write_display_data(logo_0[i][j]);
                #endif
                
            }
        } else {
            for ( uint8_t j=0; j<32&&xlen_t>0; j++ ) {
                xlen_t--;
                if ( i == 0 ) {
                    #if COLOR_MODE
                    st7571_write_display_data(logo_0[i][j]<<yu);
                    #else
                    st7571_write_display_data(logo_0[i][j]<<yu);
                    st7571_write_display_data(logo_0[i][j]<<yu);
                    #endif
                } else {
                    #if COLOR_MODE
                    st7571_write_display_data(logo_0[i-1][j]>>(8-yu)|logo_0[i][j]<<yu);
                    #else
                    st7571_write_display_data(logo_0[i-1][j]>>(8-yu)|logo_0[i][j]<<yu);
                    st7571_write_display_data(logo_0[i-1][j]>>(8-yu)|logo_0[i][j]<<yu);
                    #endif
                }
            } 
        }
    }
}


/**
 * 屏幕显示区: 长128 x 高96+1 (分12页显示,底部多一像素,共12*8+1像素)
 * 开始显示的数据行为0时,顶部是从 page 4页开始显示(若想page0页的数据在开头显示,则需要设置开始显示行为96)
 * RAM区: 长128 x 宽128 (上下数据循环显示,例如设置开始显示的数据行为7,底部会出现page 0页的数据)
*/
//绘制首页
void st7571_lcd_display_home( void ) {
    st7571_lcd_clear();
    writeFont_16x16(0, 0, "获取屏幕开发资料");
    writeFont_16x16(0, 16, "微信搜索公众号：");
    writeFont_20x20(4, 32+6, "猫狗之家电子");
    writeLogo_0(48, 64-6);
}
//绘制菜单页
void st7571_lcd_display_menu( void ) {
    st7571_lcd_clear();
    for ( int i=0; i<10; i++ ) {
        for ( int j=0; j<112; j++ ) {
            page_str.ram[i][j] = 0;
        }
    }
    //首页
    for ( int i=0; i<16; i++ ) {
        page_str.ram[4][8+i] = myFont_16[44][i];
        page_str.ram[5][8+i] = myFont_16[45][i];
        page_str.ram[4][24+i] = myFont_16[46][i];
        page_str.ram[5][24+i] = myFont_16[47][i];
    }
    //绘画
    for ( int i=0; i<16; i++ ) {
        page_str.ram[4][48+i] = myFont_16[48][i];
        page_str.ram[5][48+i] = myFont_16[49][i];
        page_str.ram[4][64+i] = myFont_16[50][i];
        page_str.ram[5][64+i] = myFont_16[51][i];
    }
    //校准
    for ( int i=0; i<16; i++ ) {
        page_str.ram[4][88+i] = myFont_16[52][i];
        page_str.ram[5][88+i] = myFont_16[53][i];
        page_str.ram[4][104+i] = myFont_16[54][i];
        page_str.ram[5][104+i] = myFont_16[55][i];
    }
    //图片
    for ( int i=0; i<16; i++ ) {
        page_str.ram[7][48+i] = myFont_16[62][i];
        page_str.ram[8][48+i] = myFont_16[63][i];
        page_str.ram[7][64+i] = myFont_16[64][i];
        page_str.ram[8][64+i] = myFont_16[65][i];
    }
    //顶部框
    for ( int i=7; i<=40; i++ ) {
        page_str.ram[3][i] |= 0x80;
    }
    for ( int i=47; i<=80; i++ ) {
        page_str.ram[3][i] |= 0x80;
        page_str.ram[6][i] |= 0x80;
    }
    for ( int i=87; i<=120; i++ ) {
        page_str.ram[3][i] |= 0x80;
    }
    //底部框
    for ( int i=7; i<=40; i++ ) {
        page_str.ram[6][i] |= 0x01;
    }
    for ( int i=47; i<=80; i++ ) {
        page_str.ram[6][i] |= 0x01;
        page_str.ram[9][i] |= 0x01;
    }
    for ( int i=87; i<=120; i++ ) {
        page_str.ram[6][i] |= 0x01;
    }
    //左1框
    page_str.ram[3][7] |= 0x80;
    page_str.ram[4][7] |= 0xFF;
    page_str.ram[5][7] |= 0xFF;
    page_str.ram[6][7] |= 0x01;
    //左2框
    page_str.ram[3][40] |= 0x80;
    page_str.ram[4][40] |= 0xFF;
    page_str.ram[5][40] |= 0xFF;
    page_str.ram[6][40] |= 0x01;
    //左3框
    page_str.ram[3][47] |= 0x80;
    page_str.ram[4][47] |= 0xFF;
    page_str.ram[5][47] |= 0xFF;
    page_str.ram[6][47] |= 0x01;
    page_str.ram[6][47] |= 0x80;
    page_str.ram[7][47] |= 0xFF;
    page_str.ram[8][47] |= 0xFF;
    page_str.ram[9][47] |= 0x01;
    //左4框
    page_str.ram[3][80] |= 0x80;
    page_str.ram[4][80] |= 0xFF;
    page_str.ram[5][80] |= 0xFF;
    page_str.ram[6][80] |= 0x01;
    page_str.ram[6][80] |= 0x80;
    page_str.ram[7][80] |= 0xFF;
    page_str.ram[8][80] |= 0xFF;
    page_str.ram[9][80] |= 0x01;
    //左5框
    page_str.ram[3][87] |= 0x80;
    page_str.ram[4][87] |= 0xFF;
    page_str.ram[5][87] |= 0xFF;
    page_str.ram[6][87] |= 0x01;
    //左6框
    page_str.ram[3][120] |= 0x80;
    page_str.ram[4][120] |= 0xFF;
    page_str.ram[5][120] |= 0xFF;
    page_str.ram[6][120] |= 0x01;
    for ( uint8_t i=3; i<10; i++ ) {
        st7571_writeDataToRAM(i, 0, 127, &page_str.ram[i][0], 0 );
    }
}
//绘制绘画页
void st7571_lcd_display_painting( void ) {
    st7571_lcd_clear();
    writeFont_16x16(0, 0, "清屏");
    writeFont_16x16(48, 0, "绘画");
    writeFont_16x16(96, 0, "返回");
    for ( int i=0; i<10; i++ ) {
        for ( int j=0; j<112; j++ ) {
            page_str.ram[i][j] = 0;
        }
    }
    for ( int i=0; i<10; i++ ) {
        page_str.ram[i][0] = 0xFF;
        page_str.ram[i][111] = 0xFF;
    }
    for ( int i=0; i<112; i++ ) {
        page_str.ram[0][i] |= 0x01;
        page_str.ram[9][i] |= 0x80;
    }
    for ( int i=0; i<10; i++ ) {
        st7571_writeDataToRAM(i+2, 8, 120-1, &page_str.ram[i][0], 0 );
    }
}
//清空绘画内容
void st7571_lcd_display_painting_clear( void ) {
    for ( int i=0; i<10; i++ ) {
        for ( int j=0; j<112; j++ ) {
            page_str.ram[i][j] = 0;
        }
    }
    for ( int i=0; i<10; i++ ) {
        page_str.ram[i][0] = 0xFF;
        page_str.ram[i][111] = 0xFF;
    }
    for ( int i=0; i<112; i++ ) {
        page_str.ram[0][i] |= 0x01;
        page_str.ram[9][i] |= 0x80;
    }
    for ( int i=0; i<10; i++ ) {
        st7571_writeDataToRAM(i+2, 8, 120-1, &page_str.ram[i][0], 0 );
    }
}
//绘画模式，显示绘制图像
void st7571_painting( uint8_t x, uint8_t y ) {
    uint8_t chu,yu;
    //绘画区域：2~12页 8~120列 x:[8,120] y:[16,96]
    if ( x>=8 && x<120 && y>=16 && y<96 ) {
        x = x-8;
        chu = (y-16)/8;
        yu = (y-16)%8;
        page_str.ram[chu][x] |= (1<<yu);
        //设置页地址
        st7571_set_page_address(chu+2);
        //设置列地址
        st7571_set_column_address(x+8);
        //写入数据
        #if COLOR_MODE
        st7571_write_display_data(page_str.ram[chu][x]);
        #else
        st7571_write_display_data(page_str.ram[chu][x]);
        st7571_write_display_data(page_str.ram[chu][x]);
        #endif
    }
}
//绘制灰度图片
void st7571_lcd_display_img( void ) {
    st7571_lcd_clear();
    #if COLOR_MODE//0灰度1黑白
    writeLogo_0(48, 32);//居中显示
    #else
    for ( int i=0; i<12; i++ ) {
        st7571_writeDataToRAM(i, 0, 127, (uint8_t *)testImg[i], 1);
    }
    #endif
}
//绘制校准页面
void st7571_lcd_display_touch_calibration( void ) {
    st7571_lcd_clear();
    writeFont_24x24(40, 0, "校准");
    writeFont_24x24(4, 24, "请点击屏幕");
    writeFont_24x24(4, 48, "四角出现的");
    writeFont_24x24(16, 72, "“圆点”");
    //绘制左上角圆点
    st7571_writeDataToRAM(0, 0, 9, (uint8_t *)graphDot[0], 0);
    st7571_writeDataToRAM(1, 0, 9, (uint8_t *)graphDot[1], 0);
}














