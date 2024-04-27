#include "quantum.h"
#include "rgb_matrix.h"

extern void transferRgbColorsToSpi(void);

void housekeeping_task_kb(void) {
    transferRgbColorsToSpi();
}


/*
 * must override sleep functions without idle thread
 */
#if CH_CFG_NO_IDLE_THREAD == TRUE

#    define CYCLES_PER_LOOP 9
#    define LOOP_TIMES (48000000 / (CH_CFG_ST_FREQUENCY) / (CYCLES_PER_LOOP))

void chThdSleep(sysinterval_t time) {
    uint32_t loops = time * LOOP_TIMES;

    for (uint32_t i = 0; i < loops; i++) __NOP();
}

/* suspend thread used in usb_main.c */
msg_t chThdSuspendTimeoutS(thread_reference_t *trp, sysinterval_t timeout) {
    osalSysUnlock();
    wait_ms(1);
    //while (usbGetTransmitStatusI(&USBD1, KEYBOARD_IN_EPNUM));
#ifdef SHARED_EP_ENABLE
    //while (usbGetTransmitStatusI(&USBD1, SHARED_IN_EPNUM));
#endif
    osalSysLock();

    return MSG_OK;
}

#endif /* CH_CFG_NO_IDLE_THREAD */

//int rand(void) {
//    static uint32_t seed = 134775813U;
//    seed                 = seed * 1664525U + 1013904223U;
//    return seed;
//}


