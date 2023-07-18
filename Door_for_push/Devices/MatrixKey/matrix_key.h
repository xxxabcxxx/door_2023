#ifndef MATRIX_KEY_H_
#define MATRIX_KEY_H_
#include <stdint.h>
#include "drv_gpio.h"
#include "ch32v30x.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"
#include "function.h"

typedef struct MatrixKey
{
  uint8_t pressed_pin_;
  uint8_t key_num_;
  // 每隔一行所需增加的数（启用了x个列则将此值设为x）
  uint8_t row_offset_;
#if USE_MATRIXKEY_REMEMBER_THE_LAST_VAL
  uint8_t last_key_num_;
#endif
  uint8_t using_row0;
  uint8_t using_row1;
  uint8_t using_row2;
  uint8_t using_row3;
  uint8_t using_col0;
  uint8_t using_col1;
  uint8_t using_col2;
  uint8_t using_col3;              
GPIO_TypeDef  * gpio_channel_row0,
              * gpio_channel_row1,
              * gpio_channel_row2,
              * gpio_channel_row3,
              * gpio_channel_col0,
              * gpio_channel_col1,
              * gpio_channel_col2,
              * gpio_channel_col3;
uint16_t            gpio_pin_row0,
                    gpio_pin_row1,
                    gpio_pin_row2,
                    gpio_pin_row3,  
                    gpio_pin_col0,
                    gpio_pin_col1,
                    gpio_pin_col2,
                    gpio_pin_col3;             

}MatrixKey;

extern MatrixKey g_matrix_key;


uint8_t GetMartixKeyNum(MatrixKey* matrix);


void InitMatrixKey(MatrixKey* key);

#endif
