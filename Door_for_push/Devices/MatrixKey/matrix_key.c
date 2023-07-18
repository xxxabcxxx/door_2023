#include "matrix_key.h"

MatrixKey g_matrix_key;

int MatrixKeyRowPinScan(MatrixKey* matrixkey)
{
    if (matrixkey->using_row0)
    {
        if (GPIO_ReadInputDataBit(matrixkey->gpio_channel_row0, matrixkey->gpio_pin_row0) == matrixkey->pressed_pin_)
            return 1;
    }
    if (matrixkey->using_row1)
    {
        if (GPIO_ReadInputDataBit(matrixkey->gpio_channel_row1, matrixkey->gpio_pin_row1) == matrixkey->pressed_pin_)
            return 2;        
    }
    if (matrixkey->using_row2)
    {
        if (GPIO_ReadInputDataBit(matrixkey->gpio_channel_row2, matrixkey->gpio_pin_row2) == matrixkey->pressed_pin_)
            return 3;        
    }
    if (matrixkey->using_row3)
    {
        if (GPIO_ReadInputDataBit(matrixkey->gpio_channel_row3, matrixkey->gpio_pin_row3) == matrixkey->pressed_pin_)
            return 4;              
    }
    return 0;                 
}

uint8_t GetMartixKeyNum(MatrixKey* matrixkey)
{   
    uint8_t key_row_num = 0;
    matrixkey->key_num_ = 0;
    if (matrixkey->using_col0)
    {
        GPIO_ResetBits(matrixkey->gpio_channel_col0, matrixkey->gpio_pin_col0);
        if ((key_row_num = MatrixKeyRowPinScan(matrixkey)))
        {
            while((MatrixKeyRowPinScan(matrixkey)));
            matrixkey->key_num_ = (key_row_num - 1) * matrixkey->row_offset_ + 1;
        }
        GPIO_SetBits(matrixkey->gpio_channel_col0, matrixkey->gpio_pin_col0);
    }   

    if (matrixkey->using_col1)
    {
        GPIO_ResetBits(matrixkey->gpio_channel_col1, matrixkey->gpio_pin_col1);
        if ((key_row_num = MatrixKeyRowPinScan(matrixkey)))
        {
            while((MatrixKeyRowPinScan(matrixkey)));
            matrixkey->key_num_ = (key_row_num - 1) * matrixkey->row_offset_ + 2; 
        }
        GPIO_SetBits(matrixkey->gpio_channel_col1, matrixkey->gpio_pin_col1);
    }

    if (matrixkey->using_col2)
    {
        GPIO_ResetBits(matrixkey->gpio_channel_col2, matrixkey->gpio_pin_col2);
        if ((key_row_num = MatrixKeyRowPinScan(matrixkey)))
        {
            while((MatrixKeyRowPinScan(matrixkey)));
            matrixkey->key_num_ = (key_row_num - 1) * matrixkey->row_offset_ + 3; 
        }
        GPIO_SetBits(matrixkey->gpio_channel_col2, matrixkey->gpio_pin_col2);
    }
  
    if (matrixkey->using_col3)
    {
        GPIO_WriteBit(matrixkey->gpio_channel_col3, matrixkey->gpio_pin_col3, matrixkey->pressed_pin_);
        if ((key_row_num = MatrixKeyRowPinScan(matrixkey)))
        {
            while((MatrixKeyRowPinScan(matrixkey)));
            matrixkey->key_num_ = (key_row_num - 1) * matrixkey->row_offset_ + 4; 
        }
        GPIO_WriteBit(matrixkey->gpio_channel_col3, matrixkey->gpio_pin_col3, !(matrixkey->pressed_pin_));            
    }
    #if USE_MATRIXKEY_REMEMBER_THE_LAST_VAL
        if (matrixkey->key_num_)
            matrixkey->last_key_num_ = matrixkey->key_num_;
    #endif 

    return matrixkey->key_num_;
}



void InitMatrixKey(MatrixKey* key)
{
    GPIO_InitTypeDef GPIO_InitTypdefStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitTypdefStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13;
    GPIO_InitTypdefStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitTypdefStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitTypdefStruct);
    GPIO_InitTypdefStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14;
    GPIO_InitTypdefStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitTypdefStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitTypdefStruct);

    GPIO_SetBits(GPIOE, GPIO_Pin_13);
    GPIO_SetBits(GPIOE, GPIO_Pin_11);
    GPIO_SetBits(GPIOE, GPIO_Pin_9);


    key->pressed_pin_ = RESET;

    key->using_col0 = TRUE;
    key->gpio_pin_col0 = GPIO_Pin_13;
    key->gpio_channel_col0 = GPIOE;

    key->using_col1 = TRUE;
    key->gpio_pin_col1 = GPIO_Pin_11;
    key->gpio_channel_col1 = GPIOE;

    key->using_col2 = TRUE;
    key->gpio_pin_col2 = GPIO_Pin_9;
    key->gpio_channel_col2 = GPIOE;

    key->using_col3 = FALSE;

    key->row_offset_ = 3;

    key->using_row0 = TRUE;
    key->gpio_pin_row0 = GPIO_Pin_14;
    key->gpio_channel_row0 = GPIOE;

    key->using_row1 = TRUE;
    key->gpio_pin_row1 = GPIO_Pin_12;
    key->gpio_channel_row1 = GPIOE;

    key->using_row2 = TRUE;
    key->gpio_pin_row2 = GPIO_Pin_10;
    key->gpio_channel_row2 = GPIOE;

    key->using_row3 = TRUE;
    key->gpio_pin_row3 = GPIO_Pin_8;
    key->gpio_channel_row3 = GPIOE;

    

}