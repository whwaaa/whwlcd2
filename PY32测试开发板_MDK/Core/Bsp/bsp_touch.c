#include "bsp_touch.h"

PRESS_STR press_str = {0};

/**
 * 检测x坐标
 */
uint32_t touch_check_x( void ) {
    uint32_t vol, x;
    
    TOUCH_X_PWR_ON;

    //启动adc
    if ( HAL_ADC_Start( &hadc ) != 0 ) {
        // printf("adc err\n");
    }

    /*规则转换先转换Channel_0这个通道，值不是需要的*/
    HAL_ADC_PollForConversion( &hadc, 1000 );
    HAL_ADC_GetValue( &hadc );

    /*第二次获取的是Channel_1,是需要的值*/
    //等待转换结束
    if ( HAL_ADC_PollForConversion( &hadc, 1000 ) != 0 ) {
        // printf("adc poll err\n");
    }
    vol = HAL_ADC_GetValue( &hadc );
    x = 128*(vol-udata.uxGnd)/(udata.uxVcc-udata.uxGnd);
    if ( x > 128 ) x = 128;
    x = 128 - x;
    press_str.x = x;
    
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
    if ( HAL_ADC_Start( &hadc ) != 0 ) {
        // printf("adc err\n");
    }
    //等待转换结束
    if ( HAL_ADC_PollForConversion( &hadc, 1000 ) != 0 ) {
        // printf("adc poll err\n");
    }
    //查询获取adc值
    vol = HAL_ADC_GetValue( &hadc );//第一次获取的是Channel_0

    // /*第二次获取的是Channel_1,不是需要的值*/
    // HAL_ADC_PollForConversion( &hadc, 1000 )
    // HAL_ADC_GetValue( &hadc );

    y = 96*(vol-udata.uyGnd)/(udata.uyVcc-udata.uyGnd);
    if ( y > 96 ) y = 96;
    y = 96 - y;
    press_str.y = y;
    
    TOUCH_Y_PWR_OFF;
    return vol;
}

uint16_t my_abs_diff( int32_t a, int32_t b ) {
    if ( a - b < 0 ) {
        return (uint16_t)(b - a);
    } else {
        return (uint16_t)(a - b);
    }
}



