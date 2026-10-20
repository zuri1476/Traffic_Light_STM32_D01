/*
 * led7seg_595.c
 *
 * Driver LED 7 doan 4 so Anode chung qua 74HC595 (quet ghep kenh).
 * Xem mo ta chan va cach dung trong led7seg_595.h
 */

#include "led7seg_595.h"

/* ---- Cac chan dieu khien (tat ca nam tren GPIOB) ------------------------ */
#define DIG_ALL_MASK   (DIG1_Pin | DIG2_Pin | DIG3_Pin | DIG4_Pin)

/* Ghi chan nhanh qua BSRR (nguyen tu, an toan khi goi trong ngat) */
#define PIN_HIGH(pin)  (GPIOB->BSRR = (uint32_t)(pin))
#define PIN_LOW(pin)   (GPIOB->BSRR = ((uint32_t)(pin)) << 16U)

/* Ma 7 doan Anode chung: bit0=a ... bit6=g, bit7=dp ; muc 0 = sang */
static const uint8_t SEG_CODE[10] = {
    0xC0, /* 0 */
    0xF9, /* 1 */
    0xA4, /* 2 */
    0xB0, /* 3 */
    0x99, /* 4 */
    0x92, /* 5 */
    0x82, /* 6 */
    0xF8, /* 7 */
    0x80, /* 8 */
    0x90  /* 9 */
};
#define SEG_BLANK      0xFF   /* tat het doan */

/* Chan transistor chon LED tuong ung vi tri 0..3 */
static const uint16_t DIG_PIN[4] = { DIG1_Pin, DIG2_Pin, DIG3_Pin, DIG4_Pin };

/* Bo dem hien thi: [0]=chuc A, [1]=don vi A, [2]=chuc B, [3]=don vi B */
static volatile uint8_t digit_buf[4] = { 0, 0, 0, 0 };
static volatile uint8_t display_on   = 1;
static uint8_t scan_idx = 0;

/* ---- Ham noi bo --------------------------------------------------------- */

/* Tat ca 4 transistor (muc 1 = tat) de tranh bong mo khi doi ma */
static inline void digits_off(void)
{
    PIN_HIGH(DIG_ALL_MASK);
}

/* Dich 8 bit (MSB truoc) vao 74HC595 roi chot ra ngo ra */
static void hc595_send(uint8_t data)
{
    for (int8_t i = 7; i >= 0; i--)
    {
        if ((data >> i) & 0x01U) { PIN_HIGH(LED7_DS_Pin); }
        else                     { PIN_LOW(LED7_DS_Pin);  }

        PIN_HIGH(SHCP_Pin);     /* xung dich SH_CP */
        PIN_LOW(SHCP_Pin);
    }
    PIN_HIGH(STCP_Pin);         /* xung chot ST_CP */
    PIN_LOW(STCP_Pin);
}

/* ---- API ---------------------------------------------------------------- */

void led7seg_init(void)
{
    /* PB3 (DIG4) va PB4 (DS) mac dinh la JTDO/JNTRST. Chuyen sang SWD-only
     * de dung duoc lam GPIO (van debug/nap bang ST-Link qua SWD binh thuong). */
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_SWJ_NOJTAG();

    /* Cau hinh GPIO da duoc MX_GPIO_Init() (CubeMX) thuc hien, chi dat muc dau */
    digits_off();
    PIN_LOW(SHCP_Pin);
    PIN_LOW(STCP_Pin);
    PIN_LOW(LED7_DS_Pin);
    hc595_send(SEG_BLANK);

    digit_buf[0] = digit_buf[1] = digit_buf[2] = digit_buf[3] = 0;
    display_on = 1;
    scan_idx   = 0;
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

void led7seg_enable(uint8_t on)
{
    display_on = (on != 0U) ? 1U : 0U;
    if (!display_on)
    {
        digits_off();
    }
}

void led7seg_scan(void)
{
    digits_off();                       /* 1. tat LED cu, chong bong mo */

    if (display_on)
    {
        hc595_send(SEG_CODE[digit_buf[scan_idx]]);  /* 2. day ma doan moi */
        PIN_LOW(DIG_PIN[scan_idx]);                 /* 3. bat LED tuong ung */
    }

    scan_idx = (uint8_t)((scan_idx + 1U) & 0x03U);
}