#include "bsp_st7565.h"
#include "bsp_font.h"

//SPI接口写数据
void writeByteData( uint8_t data ) {
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);//CS_L
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);//1数据
    HAL_SPI_Transmit( &hspi1, &data, 1, 1000 );
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);//CS_H
}

//SPI接口写命令
void writeByteCmd( uint8_t cmd ) {
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);//CS_L
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);//0命令
    HAL_SPI_Transmit( &hspi1, &cmd, 1, 1000 );
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);//CS_H
}

/**
 * 清屏，清空 RAM 48*132bit
 * 分6个页，每页数据8bit*132 (其中132列地址，显示区只显示0~63的数据)
 */
void lcd_clear( void ) {
    uint8_t data;
    for ( uint8_t page=0; page<8; page++ ) {
        //设置页地址
        page_address_set(page);
        //设置列地址
        column_address_set(0);
        //写入数据
        for ( int i=0; i<132; i++ ) {
            display_data_write(0x00);
        }
    }
}

/**
 * LCD初始化配置
 */
void lcd_init( void ) {
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
    HAL_Delay(200);

    reset();//复位
    HAL_Delay(100);
    power_controller_set(4);//Voltage Coboost ON 
    HAL_Delay(10);
    power_controller_set(6);//Voltage Regulator ON 
    HAL_Delay(10);
    power_controller_set(7);//Voltage Follower ON 
    HAL_Delay(10);
    //1.效果最佳
    //lcd_bias_set(1);
    //v5_voltage_regulator_internal_resistor_ratio_set(2);
    //2.次之
    //lcd_bias_set(0);
    //v5_voltage_regulator_internal_resistor_ratio_set(4);
    lcd_bias_set(1);//默认设置Bias 1/9
    HAL_Delay(10);
    v5_voltage_regulator_internal_resistor_ratio_set(2);//电阻率选择
    HAL_Delay(10);
    the_electronic_volume(0);//对比度设置
    HAL_Delay(10);
    display_start_line_set(0);//初始化显示行0-63
    HAL_Delay(10);
    //display_all_points_ON_OFF(1);
    common_output_mode_select(1);//正方向
    display_on_off(1);//开启显示
    // display_normal_reverse(1);//反显
    lcd_clear();
}

/**
 * 指定位置写入数据
 * page：取值0~5共6页数据页
 * startX：起始x坐标，取值0~63
 * endX：最后x坐标，必须大于startX，取值0~63
 * pdata：数据长度必须大于等于(startX-endX+1)
 */
void writeDataToRAM( uint8_t page, uint8_t startX, uint8_t endX, uint8_t *pdata ) {
    uint8_t xlen;
    if ( startX>endX || endX==0 || endX>63 ) return;
    xlen = endX - startX + 1;
    //设置页地址
    page_address_set(page);
    //设置列地址
    column_address_set(startX);
    //写入数据
    for ( uint8_t i=0; i<xlen; i++ ) {
        writeByteData(pdata[i]);
    }
}

/**
 * 显示8x16大小ASCII字符
 * x取值：0~63
 * y取值：0~47
 * data：显示的数据
 * isBold：1粗体，0非粗体
*/
void writeFont_ASCII8x16( uint8_t x, uint8_t y, char *data, uint8_t isBold ) {
    uint8_t page, fontLen, yu, xlen_t, (*asciiFont_t)[16], pNum;
    uint16_t xlen;

    xlen = strlen(data) * 8;//utf8一个字3字节
    xlen = xlen > (64 - x) ? (64 - x) : xlen;
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

    for ( uint8_t i=0; i<pNum&&page<6; i++,page++ ) {
        xlen_t = xlen;
        //设置页地址
        page_address_set(page);
        //设置列地址
        column_address_set(x);
        if ( yu == 0 ) {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(asciiFontIdx)); idx++ ) {
                     if ( data[b] == asciiFontIdx[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<8&&xlen_t>0; j++ ) {
                            xlen_t--;
                            writeByteData(asciiFont_t[idx][i*8+j]);
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
                                writeByteData(asciiFont_t[idx][i*8+j]<<yu);
                            } else if ( i == 1 ) {
                                writeByteData(asciiFont_t[idx][j]>>(8-yu)|asciiFont_t[idx][i*8+j]<<yu);
                            } else if ( i == 2 ) {
                                writeByteData(asciiFont_t[idx][j]>>(8-yu));
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
 * x取值：0~63
 * y取值：0~47
 * font：显示的数据
*/
void writeFont_16x16( uint8_t x, uint8_t y, char *font ) {
    uint8_t page, fontLen, yu, pNum;
    uint16_t xlen, xlen_t;

    xlen = strlen(font) / 3 * 16;//utf8一个字3字节
    xlen = xlen > (64 - x) ? (64 - x) : xlen;
    fontLen = (xlen+15)/16;//字数长度
    //计算RAM页地址(0~3)
    page = y / 8;
    yu = y % 8;
    if ( yu == 0 ) {
        pNum = 2;//一个字高16占2页
    } else {
        pNum = 3;//一个字高16占3页
    }
    
    for ( uint8_t i=0; i<pNum&&page<6; i++,page++ ) {
        xlen_t = xlen;
        //设置页地址
        page_address_set(page);
        //设置列地址
        column_address_set(x);

        if ( yu == 0 ) {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(myFontIdx_16)/3); idx++ ) {
                    if ( (uint32_t)(font[b*3]<<16|font[b*3+1]<<8|font[b*3+2]) == myFontIdx_16[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<16&&xlen_t>0; j++ ) {
                            xlen_t--;
                            writeByteData(myFont_16[idx*2+i][j]);
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
                                writeByteData(myFont_16[idx*2+i][j]<<yu);
                            } else if ( i == 1 ){
                                writeByteData(myFont_16[idx*2+i-1][j]>>(8-yu)|(myFont_16[idx*2+i][j]<<yu));
                            } else if ( i == 2 ) {
                                writeByteData(myFont_16[idx*2+i-1][j]>>(8-yu));
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
 * 显示24x24字符
 * x取值：0~63
 * y取值：0~47
 * font：显示的数据
*/
void writeFont_24x24( uint8_t x, uint8_t y, char *font ) {
    uint8_t page, fontLen, yu, pNum;
    uint16_t xlen, xlen_t;

    xlen = strlen(font) / 3 * 24;//utf8一个字3字节
    xlen = xlen > (64 - x) ? (64 - x) : xlen;
    fontLen = (xlen+23)/24;//字数长度
    //计算RAM页地址(0~3)
    page = y / 8;
    yu = y % 8;
    if ( yu == 0 ) {
        pNum = 3;//一个字高24占3页
    } else {
        pNum = 4;//一个字高24占4页
    }

    for ( uint8_t i=0; i<pNum&&page<6; i++,page++ ) {
        xlen_t = xlen;
        //设置页地址
        page_address_set(page);
        //设置列地址
        column_address_set(x);

        if ( yu == 0 ) {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(myFontIdx_24)/3); idx++ ) {
                    if ( (uint32_t)(font[b*3]<<16|font[b*3+1]<<8|font[b*3+2]) == myFontIdx_24[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<24&&xlen_t>0; j++ ) {
                            xlen_t--;
                            writeByteData(myFont_24[idx*3+i][j]);
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
                                writeByteData(myFont_24[idx*3+i][j]<<yu);
                            } else if ( i == 1 ) {
                                writeByteData((myFont_24[idx*3+(i-1)][j]>>(8-yu))|(myFont_24[idx*3+i][j]<<yu));
                            } else if ( i == 2 ) {
                                writeByteData((myFont_24[idx*3+(i-1)][j]>>(8-yu))|(myFont_24[idx*3+i][j]<<yu));
                            } else if ( i == 3 ) {
                                writeByteData((myFont_24[idx*3+(i-1)][j]>>(8-yu)));
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
 * 显示32x32字符
 * x取值：0~63
 * y取值：0~47
 * font：显示的数据
*/
void writeFont_32x32( uint8_t x, uint8_t y, char *font ) {
    uint8_t page, fontLen, yu, pNum;
    uint16_t xlen, xlen_t;

    xlen = strlen(font) / 3 * 32;//utf8一个字3字节
    xlen = xlen > (64 - x) ? (64 - x) : xlen;
    fontLen = (xlen+31)/32;//字数长度
    //计算RAM页地址(0~3)
    page = y / 8;
    yu = y % 8;
    if ( yu == 0 ) {
        pNum = 4;//一个字高32占4页
    } else {
        pNum = 5;//一个字高32占5页
    }
    
    for ( uint8_t i=0; i<pNum&&page<6; i++,page++ ) {
        xlen_t = xlen;
        //设置页地址
        page_address_set(page);
        //设置列地址
        column_address_set(x);
        printf("yu:%d fontLen:%d\n", yu, fontLen);
        if ( yu == 0 ) {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                printf("查找font索引\n");
                for ( uint8_t idx=0; idx<(sizeof(myFontIdx_32)/3); idx++ ) {
                    if ( (uint32_t)(font[b*3]<<16|font[b*3+1]<<8|font[b*3+2]) == myFontIdx_32[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<32&&xlen_t>0; j++ ) {
                            xlen_t--;
                            writeByteData(myFont_32_black[idx*4+i][j]);
                            printf("0输出:%02X\n");
                        }
                        break;
                    }
                }
            }
        } else {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(myFontIdx_32)/3); idx++ ) {
                    if ( (uint32_t)(font[b*3]<<16|font[b*3+1]<<8|font[b*3+2]) == myFontIdx_32[idx] ) {//查找到font索引
                        //写入一个字font数据
                        for ( uint8_t j=0; j<32&&xlen_t>0; j++ ) {
                            xlen_t--;
                            if ( i == 0 ) {
                                writeByteData(myFont_32[idx*4+i][j]<<yu);
                            } else if ( i == 1 ) {
                                writeByteData((myFont_32[idx*4+(i-1)][j]>>(8-yu))|(myFont_32[idx*4+i][j]<<yu));
                            } else if ( i == 2 ) {
                                writeByteData((myFont_32[idx*4+(i-1)][j]>>(8-yu))|(myFont_32[idx*4+i][j]<<yu));
                            } else if ( i == 3 ) {
                                writeByteData((myFont_32[idx*4+(i-1)][j]>>(8-yu))|(myFont_32[idx*4+i][j]<<yu));
                            } else if ( i == 4 ) {
                                writeByteData((myFont_32[idx*4+(i-1)][j]>>(8-yu)));
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
 * 滑动显示32x32字符
 * width取值：0~63
 * x取值：0~63
 * y取值：0~47
 * font：显示的数据
 * fIdx：显示数据位置偏移量
*/
void slideFont32x32( uint8_t width, uint8_t x, uint8_t y, char *font, uint16_t fIdx ) {
    uint8_t page, fontLen, yu, pNum, offset;
    uint16_t xlen, xlen_t;
    char *font_t;
    xlen = strlen(font) / 3 * 32;//utf8一个字3字节
    if ( xlen > fIdx ) {
        xlen -= fIdx;
    } else {
        xlen = 0;    
    }
    xlen = xlen > width ? width : xlen;
    font_t = font + (fIdx/32)*3;//从font_t开始取字符
    offset = fIdx % 32;//显示字模偏移量
    fontLen = (xlen+offset+31)/32;//字数长度
    //计算RAM页地址(0~3)
    page = y / 8;
    yu = y % 8;
    if ( yu == 0 ) {
        pNum = 4;//一个字高32占4页
    } else {
        pNum = 5;//一个字高32占5页
    }
    
    for ( uint8_t i=0; i<pNum&&page<6; i++,page++ ) {
        xlen_t = xlen;
        //设置页地址
        page_address_set(page);
        //设置列地址
        column_address_set(x);
        if ( yu == 0 ) {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(myFontIdx_32)/3); idx++ ) {
                    if ( (uint32_t)(font_t[b*3]<<16|font_t[b*3+1]<<8|font_t[b*3+2]) == myFontIdx_32[idx] ) {//查找到font索引
                        //写入一个字font数据
                        if ( b == 0 ) {
                            for ( uint8_t j=offset; j<32&&xlen_t>0; j++ ) {
                                xlen_t--;
                                writeByteData(myFont_32_black[idx*4+i][j]);
                                printf("--0 xlen:%d   fontLen:%d   b:%d   offset:%d   j:%d\n", xlen, fontLen, b, offset, j);
                            }
                        } else {
                            for ( uint8_t j=0; j<32&&xlen_t>0; j++ ) {
                                xlen_t--;
                                writeByteData(myFont_32_black[idx*4+i][j]);
                                printf("--1 xlen:%d   fontLen:%d   b:%d   offset:%d   j:%d\n", xlen, fontLen, b, offset, j);
                            }
                        }
                        
                        break;
                    }
                }
            }
        } else {
            //遍历所有字
            for ( uint8_t b=0; b<fontLen; b++ ) {
                //查找font索引
                for ( uint8_t idx=0; idx<(sizeof(myFontIdx_32)/3); idx++ ) {
                    if ( (uint32_t)(font[b*3]<<16|font[b*3+1]<<8|font[b*3+2]) == myFontIdx_32[idx] ) {//查找到font索引
                        //写入一个字font数据
                        if ( b == 0 ) {
                            for ( uint8_t j=offset; j<32&&xlen_t>0; j++ ) {
                                xlen_t--;
                                if ( i == 0 ) {
                                    writeByteData(myFont_32[idx*4+i][j]<<yu);
                                } else if ( i == 1 ) {
                                    writeByteData((myFont_32[idx*4+(i-1)][j]>>(8-yu))|(myFont_32[idx*4+i][j]<<yu));
                                } else if ( i == 2 ) {
                                    writeByteData((myFont_32[idx*4+(i-1)][j]>>(8-yu))|(myFont_32[idx*4+i][j]<<yu));
                                } else if ( i == 3 ) {
                                    writeByteData((myFont_32[idx*4+(i-1)][j]>>(8-yu))|(myFont_32[idx*4+i][j]<<yu));
                                } else if ( i == 4 ) {
                                    writeByteData((myFont_32[idx*4+(i-1)][j]>>(8-yu)));
                                }
                            }
                        } else {
                            for ( uint8_t j=0; j<32&&xlen_t>0; j++ ) {
                                xlen_t--;
                                if ( i == 0 ) {
                                    writeByteData(myFont_32[idx*4+i][j]<<yu);
                                } else if ( i == 1 ) {
                                    writeByteData((myFont_32[idx*4+(i-1)][j]>>(8-yu))|(myFont_32[idx*4+i][j]<<yu));
                                } else if ( i == 2 ) {
                                    writeByteData((myFont_32[idx*4+(i-1)][j]>>(8-yu))|(myFont_32[idx*4+i][j]<<yu));
                                } else if ( i == 3 ) {
                                    writeByteData((myFont_32[idx*4+(i-1)][j]>>(8-yu))|(myFont_32[idx*4+i][j]<<yu));
                                } else if ( i == 4 ) {
                                    writeByteData((myFont_32[idx*4+(i-1)][j]>>(8-yu)));
                                }
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
 * x取值：0~63
 * y取值：0~47
 * data：显示的数据
*/
void writeLogo_0( uint8_t x, uint8_t y ) {
    uint8_t page, xlen, xlen_t, yu;

    xlen = 64 - x;
    page = y / 8;
    yu = y % 8;

    for ( uint8_t i=0; i<4&&page<6; i++,page++ ) {
        xlen_t = xlen;
        //设置页地址
        page_address_set(page);
        //设置列地址
        column_address_set(x);
        if ( yu == 0 ) {
            for ( uint8_t j=0; j<32&&xlen_t>0; j++ ) {
                xlen_t--;
                writeByteData(logo_0[i][j]);
            }
        } else {
            for ( uint8_t j=0; j<32&&xlen_t>0; j++ ) {
                xlen_t--;
                if ( i == 0 ) {
                    writeByteData(logo_0[i][j]<<yu);
                } else {
                    writeByteData(logo_0[i-1][j]>>(8-yu)|logo_0[i][j]<<yu);
                }
            } 
        }
    }
}


//64*48
uint8_t disCache[4][25*32];
void lcd_test_display( void ) {
    uint16_t strIdx = 0;
    char *disStr = SLIDE_STR;
    uint8_t strLen = (strlen(SLIDE_STR)/3);

    for ( uint8_t b=0; b<strLen; b++ ) {
        for ( int i=0; i<(sizeof(myFont_32)/3); i++ ) {
            if ( (uint32_t)(disStr[b*3]<<16|disStr[b*3+1]<<8|disStr[b*3+2]) == myFontIdx_32[i] ) {//查找到font索引
                for ( uint8_t t=0; t<4; t++ ) {
                    for ( uint8_t t1=0; t1<32; t1++ ) {
                        disCache[t][strIdx+t1] = myFont_32_black[i*4+t][t1];
                    }
                }
                strIdx += 32;
                break;
            }
        }
    }
    for ( uint8_t t=0; t<4; t++ ) {
        for ( uint8_t t1=0; t1<32; t1++ ) {
            disCache[t][strIdx+t1] = myFont_32_black[22*4+t][t1];
        }
    }
    strIdx += 32;
    for ( uint8_t t=0; t<4; t++ ) {
        for ( uint8_t t1=0; t1<32; t1++ ) {
            disCache[t][strIdx+t1] = logo_0[t][t1];
        }
    }
    

    for ( ;; ) {
        lcd_clear();
        for ( uint16_t k=0; k<=22*32+16; k+=16 ) {
            writeDataToRAM( 0+1, 0, 63, &disCache[0][k] );
            writeDataToRAM( 1+1, 0, 63, &disCache[1][k] );
            writeDataToRAM( 2+1, 0, 63, &disCache[2][k] );
            writeDataToRAM( 3+1, 0, 63, &disCache[3][k] );
            vTaskDelay(250);
        }
        vTaskDelay(2000);

        lcd_clear();
        writeFont_16x16( 0, 0, "猫狗" );
        writeFont_16x16( 0, 16, "之家" );
        writeFont_16x16( 0, 32, "电子" );
        writeLogo_0(32,8);
        vTaskDelay(5000);
    }
}

