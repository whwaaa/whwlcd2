#include "bsp_st7571.h"

#define UXVCC 0x0F5A
#define UXGND 0x0A34
#define UYVCC 0x0E80
#define UYGND 0x01A7

/*----------------------- 触摸相关start ----------------------*/
/**
 * 检测x坐标
 */
uint32_t touch_check_x( void ) {
    uint32_t vol, x;
    
    TOUCH_X_PWR_ON;

    //启动adc
    if ( HAL_ADC_Start( &hadc2 ) != 0 ) {
        printf("adc2 err\n");
    }
    //等待转换结束
    if ( HAL_ADC_PollForConversion( &hadc2, 1000 ) != 0 ) {
        printf("adc2 poll err\n");
    }
    //查询获取adc值
    vol = HAL_ADC_GetValue( &hadc2 );
    x = 128*(vol-UXGND)/(UXVCC-UXGND);
    if ( x > 128 ) x = 128;
    x = 128 - x;

    printf("vol:%04X x坐标:%d    ", vol, x);
    
    TOUCH_X_PWR_OFF;
    return vol;
}
/**
 * 检测y坐标
 */
uint32_t touch_check_y( void ) {
    uint32_t vol, y;
    
    TOUCH_Y_PWR_ON;

    //启动adc
    if ( HAL_ADC_Start( &hadc1 ) != 0 ) {
        printf("adc1 err\n");
    }
    //等待转换结束
    if ( HAL_ADC_PollForConversion( &hadc1, 1000 ) != 0 ) {
        printf("adc1 poll err\n");
    }
    //查询获取adc值
    vol = HAL_ADC_GetValue( &hadc1 );
    y = 96*(vol-UYGND)/(UYVCC-UYGND);
    if ( y > 96 ) y = 96;
    y = 96 - y;

    printf("vol:%04X y坐标:%d\n", vol, y);
    
    TOUCH_Y_PWR_OFF;
    return vol;
}

static uint32_t my_abs_diff( int32_t a, int32_t b ) {
    if ( a - b < 0 ) {
        return b - a;
    } else {
        return a - b;
    }
}
 
void touch_calibration( void ) {
    uint32_t uxvcc1, uxgnd1, uyvcc1, uygnd1;
    uint32_t uxvcc2, uxgnd2, uyvcc2, uygnd2;
    st7571_set_display_start_line(96);//设置开始显示的数据行
    st7571_lcd_clear();
    writeFont_24x24(40, 0, "校准");
    writeFont_24x24(4, 24, "请点击屏幕");
    writeFont_24x24(4, 48, "四角出现的");
    writeFont_24x24(16, 72, "“圆点”");
    // HAL_Delay(5000);

    //左上角
    printf("左上角  ");
    st7571_writeDataToRAM(0, 0, 9, (uint8_t *)graphDot[0]);
    st7571_writeDataToRAM(1, 0, 9, (uint8_t *)graphDot[1]);
    //清
    vTaskDelay(3000);
    st7571_writeDataToRAM(0, 0, 9, (uint8_t *)graphDot[2]);
    st7571_writeDataToRAM(1, 0, 9, (uint8_t *)graphDot[2]);
    uxvcc1 = touch_check_x();
    printf("uxvcc1: %04X, ", (uint16_t)uxvcc1);
    uyvcc1 = touch_check_y();
    printf("uyvcc1: %04X\n", (uint16_t)uyvcc1);


    //左下角
    printf("左下角  ");
    st7571_writeDataToRAM(11, 0, 9, (uint8_t *)graphDot[0]);
    st7571_writeDataToRAM(12, 0, 9, (uint8_t *)graphDot[1]);
    //清
    vTaskDelay(3000);
    st7571_writeDataToRAM(11, 0, 9, (uint8_t *)graphDot[2]);
    st7571_writeDataToRAM(12, 0, 9, (uint8_t *)graphDot[2]);
    uxvcc2 = touch_check_x();
    printf("uxvcc2: %04X, ", (uint16_t)uxvcc2);
    uygnd1 = touch_check_y();
    printf("uygnd1: %04X\n", (uint16_t)uygnd1);

    //右上角
    printf("右上角  ");
    st7571_writeDataToRAM(0, 118, 127, (uint8_t *)graphDot[0]);
    st7571_writeDataToRAM(1, 118, 127, (uint8_t *)graphDot[1]);
    //清
    vTaskDelay(3000);
    st7571_writeDataToRAM(0, 118, 127, (uint8_t *)graphDot[2]);
    st7571_writeDataToRAM(1, 118, 127, (uint8_t *)graphDot[2]);
    uxgnd1 = touch_check_x();
    printf("uxgnd1: %04X, ", (uint16_t)uxgnd1);
    uyvcc2 = touch_check_y();
    printf("uyvcc2: %04X\n", (uint16_t)uyvcc2);


    //右下角
    printf("右下角  ");
    st7571_writeDataToRAM(11, 118, 127, (uint8_t *)graphDot[0]);
    st7571_writeDataToRAM(12, 118, 127, (uint8_t *)graphDot[1]);
    //清
    vTaskDelay(3000);
    st7571_writeDataToRAM(11, 118, 127, (uint8_t *)graphDot[2]);
    st7571_writeDataToRAM(12, 118, 127, (uint8_t *)graphDot[2]);
    uxgnd2 = touch_check_x();
    printf("uxgnd2: %04X, ", (uint16_t)uxgnd2);
    uygnd2 = touch_check_y();
    printf("uygnd2: %04X\n", (uint16_t)uygnd2);


    if ( my_abs_diff((int32_t)uxvcc1, (int32_t)uxvcc2) < 0xff &&
         my_abs_diff((int32_t)uxgnd1, (int32_t)uxgnd2) < 0x1ff &&
         my_abs_diff((int32_t)uyvcc1, (int32_t)uyvcc2) < 0xff &&
         my_abs_diff((int32_t)uygnd1, (int32_t)uygnd2) < 0xff ) {
        
        printf("校验成功!\nuxvcc:%04X uxgnd:%04X uyvcc:%04X uygnd:%04X\n",
            (uint16_t)((uxvcc1+uxvcc2)/2),
            (uint16_t)((uxgnd1+uxgnd2)/2),
            (uint16_t)((uyvcc1+uyvcc2)/2),
            (uint16_t)((uygnd1+uygnd2)/2) );

    } else {
       printf("校验失败请重新尝试！\n");
    }

    printf("end\n");
    
}
/*----------------------- 触摸相关end ----------------------*/




void st7571_writeByteCmd( uint8_t cmd ) {
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);//0命令
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);//CS_L
    HAL_SPI_Transmit( &hspi1, &cmd, 1, 1000 );
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);//CS_H
}

void st7571_writeByteData( uint8_t data ) {
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);//1数据
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);//CS_L
    HAL_SPI_Transmit( &hspi1, &data, 1, 1000 );
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);//CS_H
}

/**
 * 显示24x24字符
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
 * 显示20x20字符
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
 * 显示16x16字符
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
 * 显示8x16大小ASCII字符
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
        asciiFont_t = asciiFont_bold;
    } else {
        asciiFont_t = asciiFont;
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
 * 显示logo
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
void st7571_writeDataToRAM( uint8_t page, uint8_t startX, uint8_t endX, uint8_t *pdata ) {
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
        st7571_write_display_data(pdata[i]);
        st7571_write_display_data(pdata[i]);
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
    HAL_Delay(200);
    
    st7571_reset();//软件复位
    st7571_display_on_off(0);//display off
    st7571_set_mode(0x0B,0x02);//FR=1011=>85HZ, BE=1,0=>booster efficiency Level-3
    st7571_set_com_scan_direction(1);//行扫描方向(不明白,0/1设置看着没什么区别)
    st7571_set_seg_scan_direction(1);//列扫描方向
    st7571_set_display_start_line(1);//设置开始显示的数据行
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
    // st7571_entire_display_on(1);//强制开启LCD所有像素
    st7571_lcd_clear();
}

/**
 * 屏幕显示区: 长128 x 宽97 (分12页显示,底部多一像素,共12*8+1像素)
 * 开始显示的数据行为0时,顶部是从 page 4页开始显示(若想page0页的数据在开头显示,则需要设置开始显示行为96)
 * RAM区: 长128 x 宽128 (上下数据循环显示,例如设置开始显示的数据行为7,底部会出现page 0页的数据)
*/
void st7571_lcd_test_display( void ) {
    st7571_set_display_start_line(96);//设置开始显示的数据行
    for ( ;; ) {
        st7571_lcd_clear();
        for ( int i=0; i<12; i++ ) {
            st7571_writeDataToRAM(i, 0, 127, (uint8_t *)testImg[i]);
        }
        HAL_Delay(5000);
        
        st7571_lcd_clear();
        writeFont_16x16(0, 0, DIS_STR0);
        writeFont_16x16(0, 16, DIS_STR1);
        writeFont_20x20(4, 32+6, DIS_STR2);
        writeLogo_0(48, 64-6);
        HAL_Delay(5000);
        break;
    }
    // st7571_set_power_save_mode(1);//进入省电模式
    // st7571_release_power_save_mode();//退出省电模式
}












