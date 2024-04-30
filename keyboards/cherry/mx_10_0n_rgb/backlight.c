#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <SN32F240B.h>
#include "ch.h"
#include "hal.h"
#include "color.h"
#include <sn32_sys1.h>
#include <_pin_defs.h>
#include <chibios/gpio.h>

//#define USE_FRAMEBUFFER 1


#ifdef USE_FRAMEBUFFER
static uint8_t g_fb[3][144];
#endif

static uint8_t g_spi_buf[256];
static uint8_t g_led_vaf[36] = {[0 ... 35] = 0x55};

static int g_cs_pin = 1; // 1 for A5; 2 for A6; 3 for A7

bool g_indicateCapsLock = false;
bool g_indicateScrollLock = false;
bool g_indicateNumLock = false;

#ifndef USE_FRAMEBUFFER
static bool g_spi_ready = false;
#endif

/*
 * LED index to RGB address
 */
static uint8_t g_col_index[109][4] = {
    /* row 1 */
    { 0x1, 0x00, 0x0C, 0x18 },
    //{ 0x3, 0x04, 0x10, 0x1C }, // cherry key
    //{ 0x1, 0x01, 0x0D, 0x19 },
    { 0x1, 0x02, 0x0E, 0x1A },
    { 0x1, 0x03, 0x0F, 0x1B },
    { 0x1, 0x04, 0x10, 0x1C },
    { 0x1, 0x05, 0x11, 0x1D },
    { 0x1, 0x06, 0x12, 0x1E },
    { 0x1, 0x07, 0x13, 0x1F },
    { 0x1, 0x08, 0x14, 0x20 },
    { 0x1, 0x09, 0x15, 0x21 },
    { 0x1, 0x0A, 0x16, 0x22 },
    { 0x1, 0x0B, 0x17, 0x23 },
    { 0x3, 0x00, 0x0C, 0x18 },
    { 0x3, 0x01, 0x0D, 0x19 },
    { 0x3, 0x02, 0x0E, 0x1A },
    { 0x3, 0x03, 0x0F, 0x1B },
    { 0x3, 0x04, 0x10, 0x1C }, // cherry key address used for PAUSE key
    { 0x3, 0x05, 0x11, 0x1D },
    { 0x3, 0x06, 0x12, 0x1E },
    { 0x3, 0x07, 0x13, 0x1F },
    { 0x3, 0x08, 0x14, 0x20 },

    /* row 2 */
    { 0x1, 0x24, 0x30, 0x3C },
    { 0x1, 0x25, 0x31, 0x3D },
    { 0x1, 0x26, 0x32, 0x3E },
    { 0x1, 0x27, 0x33, 0x3F },
    { 0x1, 0x28, 0x34, 0x40 },
    { 0x1, 0x29, 0x35, 0x41 },
    { 0x1, 0x2A, 0x36, 0x42 },
    { 0x1, 0x2B, 0x37, 0x43 },
    { 0x1, 0x2C, 0x38, 0x44 },
    { 0x1, 0x2D, 0x39, 0x45 },
    { 0x1, 0x2E, 0x3A, 0x46 },
    { 0x1, 0x2F, 0x3B, 0x47 },
    { 0x3, 0x24, 0x30, 0x3C },
    { 0x3, 0x25, 0x31, 0x3D },
    { 0x3, 0x26, 0x32, 0x3E },
    { 0x3, 0x27, 0x33, 0x3F },
    { 0x3, 0x28, 0x34, 0x40 },
    { 0x3, 0x29, 0x35, 0x41 },
    { 0x3, 0x2A, 0x36, 0x42 },
    { 0x3, 0x2B, 0x37, 0x43 },
    { 0x3, 0x2C, 0x38, 0x44 },

    /* row 3 */
    { 0x1, 0x48, 0x54, 0x60 },
    { 0x1, 0x49, 0x55, 0x61 },
    { 0x1, 0x4A, 0x56, 0x62 },
    { 0x1, 0x4B, 0x57, 0x63 },
    { 0x1, 0x4C, 0x58, 0x64 },
    { 0x1, 0x4D, 0x59, 0x65 },
    { 0x1, 0x4E, 0x5A, 0x66 },
    { 0x1, 0x4F, 0x5B, 0x67 },
    { 0x1, 0x50, 0x5C, 0x68 },
    { 0x1, 0x51, 0x5D, 0x69 },
    { 0x1, 0x52, 0x5E, 0x6A },
    { 0x1, 0x53, 0x5F, 0x6B },
    { 0x3, 0x48, 0x54, 0x60 },
    //{ 0x3, 0x49, 0x55, 0x61 }, // ISO enter key
    { 0x3, 0x4A, 0x56, 0x62 },
    { 0x3, 0x4B, 0x57, 0x63 },
    { 0x3, 0x4C, 0x58, 0x64 },
    { 0x3, 0x4D, 0x59, 0x65 },
    { 0x3, 0x4E, 0x5A, 0x66 },
    { 0x3, 0x4F, 0x5B, 0x67 },
    { 0x3, 0x50, 0x5C, 0x68 },

    /* row 4 */
    { 0x1, 0x6C, 0x78, 0x84 },
    { 0x1, 0x6D, 0x79, 0x85 },
    { 0x1, 0x6E, 0x7A, 0x86 },
    { 0x1, 0x6F, 0x7B, 0x87 },
    { 0x1, 0x70, 0x7C, 0x88 },
    { 0x1, 0x71, 0x7D, 0x89 },
    { 0x1, 0x72, 0x7E, 0x8A },
    { 0x1, 0x73, 0x7F, 0x8B },
    { 0x1, 0x74, 0x80, 0x8C },
    { 0x1, 0x75, 0x81, 0x8D },
    { 0x1, 0x76, 0x82, 0x8E },
    { 0x1, 0x77, 0x83, 0x8F },
    { 0x3, 0x6C, 0x78, 0x84 },
    { 0x3, 0x6D, 0x79, 0x85 },
    //{ 0x0, 0x00, 0x00, 0x00 },
    //{ 0x0, 0x00, 0x00, 0x00 },
    //{ 0x0, 0x00, 0x00, 0x00 },
    { 0x3, 0x6E, 0x7A, 0x86 },
    { 0x3, 0x6F, 0x7B, 0x87 },
    { 0x3, 0x70, 0x7C, 0x88 },
    //{ 0x3, 0x71, 0x7D, 0x89 }, // unused

    /* row 5 */
    { 0x2, 0x00, 0x0C, 0x18 },
    { 0x2, 0x01, 0x0D, 0x19 },
    { 0x2, 0x02, 0x0E, 0x1A },
    { 0x2, 0x03, 0x0F, 0x1B },
    { 0x2, 0x04, 0x10, 0x1C },
    { 0x2, 0x05, 0x11, 0x1D },
    { 0x2, 0x06, 0x12, 0x1E },
    { 0x2, 0x07, 0x13, 0x1F },
    { 0x2, 0x08, 0x14, 0x20 },
    { 0x2, 0x09, 0x15, 0x21 },
    { 0x2, 0x0A, 0x16, 0x22 },
    { 0x2, 0x0B, 0x17, 0x23 },
    //{ 0x2, 0x48, 0x54, 0x60 }, // right shift
    { 0x2, 0x49, 0x55, 0x61 },
    //{ 0x0, 0x00, 0x00, 0x00 },
    { 0x2, 0x4A, 0x56, 0x62 },
    //{ 0x0, 0x00, 0x00, 0x00 },
    { 0x2, 0x4B, 0x57, 0x63 },
    { 0x2, 0x4C, 0x58, 0x64 },
    { 0x2, 0x4D, 0x59, 0x65 },
    { 0x2, 0x4E, 0x5A, 0x66 },

    /* row 6 */
    { 0x2, 0x24, 0x30, 0x3C },
    { 0x2, 0x25, 0x31, 0x3D },
    { 0x2, 0x26, 0x32, 0x3E },
    //{ 0x0, 0x00, 0x00, 0x00 },
    //{ 0x0, 0x00, 0x00, 0x00 },
    //{ 0x0, 0x00, 0x00, 0x00 },
    { 0x2, 0x28, 0x34, 0x40 },
    //{ 0x0, 0x00, 0x00, 0x00 },
    //{ 0x0, 0x00, 0x00, 0x00 },
    //{ 0x0, 0x00, 0x00, 0x00 },
    { 0x2, 0x2A, 0x36, 0x42 },
    { 0x2, 0x2B, 0x37, 0x43 },
    { 0x2, 0x2C, 0x38, 0x44 },
    { 0x2, 0x2D, 0x39, 0x45 },
    { 0x2, 0x2E, 0x3A, 0x46 },
    { 0x2, 0x2F, 0x3B, 0x47 },
    { 0x2, 0x6C, 0x78, 0x84 },
    //{ 0x0, 0x00, 0x00, 0x00 },
    { 0x2, 0x6D, 0x79, 0x85 },
    { 0x2, 0x6E, 0x7A, 0x86 },
    //{ 0x0, 0x00, 0x00, 0x00 },
};


/*****************************************************************************
 * Function		: SPI0_Init40B
 * Description	: Initialization of SPI0 init
 * Input		: None
 * Output		: None
 * Return		: None
 * Note			: None
 *****************************************************************************/
void SPI0_Init40B(void) {

    sys1EnableSPI0();

    SN_SPI0->CTRL0_b.DL = 7; // SPI_DL_8; // 8 bit Data length

    SN_SPI0->CTRL0_b.MS = 0;       // MS disable bit
    SN_SPI0->CTRL0_b.LOOPBACK = 0; // Loop back mode
    SN_SPI0->CTRL0_b.SDODIS   = 1; // Slave data out disabled

    SN_SPI0->CLKDIV_b.DIV = 0x0B; // clock divider

    SN_SPI0->CTRL0_b.SELDIS = 1; // Auto-SEL disable bit

    SN_GPIO0->MODE_b.MODE5 = 1;
    SN_GPIO0->DATA_b.DATA5 = 1;
    SN_GPIO0->MODE_b.MODE6 = 1;
    SN_GPIO0->DATA_b.DATA6 = 1;
    SN_GPIO0->MODE_b.MODE7 = 1;
    SN_GPIO0->DATA_b.DATA7 = 1;

    SN_GPIO1->MODE_b.MODE2 = 1;
    SN_GPIO1->DATA_b.DATA2 = 1;
    SN_GPIO1->MODE_b.MODE3 = 1;
    SN_GPIO1->DATA_b.DATA3 = 1;

    SN_SPI0->CTRL0_b.FRESET = 0b11;

    NVIC_DisableIRQ(SPI0_IRQn);
    
    SN_SPI0->CTRL0_b.SPIEN = 1;
}

/*****************************************************************************
 * Function		: SPI0_Enable40B
 * Description	: SPI0 enable setting
 * Input		: None
 * Output		: None
 * Return		: None
 * Note			: None
 *****************************************************************************/
void SPI0_Enable40B(void) {

    sys1EnableSPI0();
    SN_SPI0->CTRL0_b.SPIEN = 1;
    SN_SPI0->CTRL0_b.FRESET = 0b11;
}

/*****************************************************************************
 * Function		: SPI0_Disable40B
 * Description	: SPI0 disable setting
 * Input		: None
 * Output		: None
 * Return		: None
 * Note			: None
 *****************************************************************************/
void SPI0_Disable40B(void) {

    SN_SPI0->CTRL0_b.SPIEN = 0;
    sys1DisableSPI0();
}

void SPI0_Flush(void) {
    while (SN_SPI0->STAT_b.BUSY)
        ;
}

void spi_init(void) {
    SPI0_Init40B();
    SPI0_Enable40B();
}

void spi_set_cspin(int pin) {
    if (pin < 1 || pin > 3) return;
    g_cs_pin = pin;
}

void spi_cs_on(void) {
    if (g_cs_pin==1) {
        writePinLow(A5);
        writePinHigh(A6);
        writePinHigh(A7);
    } else if (g_cs_pin==2) {
        writePinHigh(A5);
        writePinLow(A6);
        writePinHigh(A7);
    } else if (g_cs_pin==3) {
        writePinHigh(A5);
        writePinHigh(A6);
        writePinLow(A7);
    }
}

void spi_cs_off(void) {
    writePinHigh(A5);
    writePinHigh(A6);
    writePinHigh(A7);
}

static uint8_t dummyData;

void __SpiSend(uint8_t *p, int len)
{
    for (int i = 0; i < len; i++)
    {
        SN_SPI0->DATA_b.Data = *p++;
        while (SN_SPI0->STAT_b.TXFIFOTHF==0);
        while (SN_SPI0->STAT_b.RXFIFOTHF == 0);
        if (SN_SPI0->STAT_b.RX_FULL==0) {
            dummyData = (uint8_t)SN_SPI0->DATA;
        }
    }
    while (SN_SPI0->STAT_b.BUSY);
    SN_SPI0->CTRL0_b.FRESET = 0b11;
}


void spi_write_raw_arr(uint8_t* data_p, int len) {
    spi_cs_on();
    __SpiSend(data_p, len);
    spi_cs_off();
}
void spi_write_addr_arr(uint8_t page, uint8_t addr, uint8_t* data_p, int len) {
    g_spi_buf[0] = page | 0x20;
    g_spi_buf[1] = addr;
    for (int i = 0; i < len; ++i)
        g_spi_buf[i + 2] = data_p[i];
    spi_write_raw_arr(g_spi_buf, len + 2);
}

void spi_w2(uint8_t page, uint8_t addr) {
    g_spi_buf[0] = page | 0x20;
    g_spi_buf[1] = addr;
    spi_write_raw_arr(g_spi_buf, 2);
}

void spi_w3(uint8_t page, uint8_t addr, uint8_t data) {
    g_spi_buf[0] = page | 0x20;
    g_spi_buf[1] = addr;
    g_spi_buf[2] = data;
    spi_write_raw_arr(g_spi_buf, 3);
}


#ifdef USE_FRAMEBUFFER
static void flush_led_fb(int32_t pin)
{
    spi_set_cspin(pin);
    spi_write_addr_arr(0, 36, g_fb[g_cs_pin-1], 144);
}
#else
static void _set_color_direct(int index, uint8_t r, uint8_t g, uint8_t b) {
    if (index<0 || index >= DRIVER_LED_TOTAL) return;
    if ((index == 61 && g_indicateCapsLock) || (index==14 && g_indicateScrollLock) || (index==37 && g_indicateNumLock)) {
        r = 255, g = 0, b = 0;
    }
    uint8_t idx = g_col_index[index][0];
    if (idx < 1 || idx > 3) return;
    spi_set_cspin(idx);
    spi_w3(0, 36 + g_col_index[index][1], r);
    spi_w3(0, 36 + g_col_index[index][2], g);
    spi_w3(0, 36 + g_col_index[index][3], b);
}
#endif // USE_FRAMEBUFFER


void _set_color(int index, uint8_t r, uint8_t g, uint8_t b) {
#ifdef USE_FRAMEBUFFER
    if (index<0 || index >= DRIVER_LED_TOTAL) return;
    if ((index == 62 && g_indicateCapsLock) || (index==14 && g_indicateScrollLock) || (index==37 && g_indicateNumLock)) {
        r = 255, g = 0, b = 0;
    }
    uint8_t indx = g_col_index[index][0];
    if (indx == 1 || indx == 2 || indx == 3) {
        g_fb[indx - 1][g_col_index[index][1]] = r;
        g_fb[indx - 1][g_col_index[index][2]] = g;
        g_fb[indx - 1][g_col_index[index][3]] = b;
    }
#else
    if (g_spi_ready)
        _set_color_direct(index, r, g, b);
#endif
}

void reset_rgb(int byteCountToSend) {
#ifdef USE_FRAMEBUFFER
    for (int j = 0; j < 144; ++j)
        g_fb[g_cs_pin-1][j] = 0;
    spi_write_addr_arr(0, 36, g_fb[g_cs_pin-1], 144);
#endif

    uint8_t arr[180] = {[0 ... 18] = 0xFF, [19 ... 179] = 0x0};
    spi_w3(0xB, 10, 0);
    spi_w3(0xB, 0, 0);
    spi_w3(0xB, 1, 24);
    spi_w3(0xB, 5, 0);
    spi_w3(0xB, 6, 0);
    spi_w3(0xB, 8, 0);
    spi_w3(0xB, 9, 0);
    spi_w3(0xB, 11, 0);
    spi_w3(0xB, 13, 15);
    spi_w3(0xB, 14, 1);
    spi_w3(0xB, 20, 68);
    spi_w3(0xB, 21, 0);
    spi_w3(0xB, 24, 170);
    spi_w3(0xB, 25, 170);
    spi_w3(0xB, 26, 170);
    spi_w3(0xB, 15, 137);
    spi_write_addr_arr(0xD, 0, g_led_vaf, 36);
    spi_write_addr_arr(0, 0, arr, byteCountToSend);
    spi_w3(0xB, 10, 1);
}


void process_backlight(uint8_t devid, volatile rgb_led_t* states) {
    static unsigned char state = 0;

    switch (state) {
        case 0: /* init RGB chips */
            spi_init();

            spi_set_cspin(1);
            reset_rgb(180);
            spi_set_cspin(2);
            reset_rgb(180);
            spi_set_cspin(3);
            reset_rgb(180);

            state = 1;
#ifndef USE_FRAMEBUFFER
            g_spi_ready = true;
#endif
            break;

        case 1:
#ifdef USE_FRAMEBUFFER
            flush_led_fb(1);
            flush_led_fb(2);
            flush_led_fb(3);
#endif
            break;
    }
}
