#include "led7seg_595.h"

/* Mã LED 7 do?n Anode chung (m?c 0 = sáng) */
static const uint8_t seg_code[10] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90
};

static volatile uint8_t digit_buf[4] = {3, 0, 2, 7};
static uint8_t scan_idx = 0;

static void digits_off(void)
{
    /* T?t toàn b? 4 transistor PNP (M?c 1 = t?t) */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_SET);
}

static void hc595_send(uint8_t data)
{
    for (int8_t i = 7; i >= 0; i--)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, ((data >> i) & 0x01U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);   /* SH_CP */
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);        /* ST_CP */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
}

void led7seg_init(void)
{
    /* T? Ð?NG C?U HÌNH TR?C TI?P CÁC CHÂN GPIO (B? QUA L?I QUÊN C?U HÌNH TRONG CUBEMX) */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* C?u hình PB0 -> PB3 cho Transistor di?u khi?n LED */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* C?u hình PB4 (DS), PB5 (SH_CP), PB6 (ST_CP) cho 74HC595 */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /* -------------------------------------------------------------------------- */

    digits_off();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    hc595_send(0xFF);   /* T?t h?t do?n */
    scan_idx = 0;
}

void led7seg_set_time(uint8_t secA, uint8_t secB)
{
    if (secA > 99U) secA = 99U;
    if (secB > 99U) secB = 99U;
    digit_buf[0] = secA / 10U;
    digit_buf[1] = secA % 10U;
    digit_buf[2] = secB / 10U;
    digit_buf[3] = secB % 10U;
}

void led7seg_scan(void)
{
    digits_off();                                     /* T?t LED cu ch?ng bóng m? */
    hc595_send(seg_code[digit_buf[scan_idx]]);        /* Ð?y mã do?n m?i */
    
    /* B?t dúng transistor PNP tuong ?ng t?i PB0 -> PB3 (M?c 0 = b?t) */
    switch (scan_idx)
    {
        case 0: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); break;
        case 1: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); break;
        case 2: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET); break;
        case 3: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); break;
    }
    
    scan_idx = (uint8_t)((scan_idx + 1U) & 0x03U);
}