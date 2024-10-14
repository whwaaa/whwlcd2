#include "spi.h"

SPI_HandleTypeDef hspi1;

void SPI_Init( void ) {
    /*初始化SPI配置*/
    hspi1.Instance               = SPI1;                       
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;    
    hspi1.Init.Direction         = SPI_DIRECTION_2LINES;       
    hspi1.Init.CLKPolarity       = SPI_POLARITY_LOW;           /* 时钟极性低 */
    hspi1.Init.CLKPhase          = SPI_PHASE_1EDGE ;           /* 数据采样从第一个时钟边沿开始 */
    hspi1.Init.DataSize          = SPI_DATASIZE_8BIT;          /* SPI数据长度为8bit */
    hspi1.Init.FirstBit          = SPI_FIRSTBIT_MSB;           /* 先发送MSB */
    hspi1.Init.NSS               = SPI_NSS_SOFT;               /* NSS软件模式 */
    hspi1.Init.Mode = SPI_MODE_MASTER;                         /* 配置为主机 */
    if (HAL_SPI_DeInit(&hspi1) != HAL_OK)                      /* SPI反初始化 */
    {
        while(1);
    }
    /*SPI初始化*/
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        while(1);
    }
}
