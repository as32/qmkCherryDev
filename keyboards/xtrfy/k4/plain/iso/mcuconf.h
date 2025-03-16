/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef MCUCONF_H
#define MCUCONF_H

/*
 * HAL driver system settings.
 */

/*
 * SN driver system settings.
 */
#define SN32_HAS_GPIOA TRUE
#define SN32_HAS_GPIOB TRUE
#define SN32_HAS_GPIOC TRUE
#define SN32_HAS_GPIOD TRUE

/*
 * USB driver system settings.
 */
#define CRT1_AREAS_NUMBER 1
#define PLATFORM_USB_USE_USB1 TRUE

/*
 * Timer driver system settings.
 */
#define SYS_CLOCK_SETUP 1
#define SYS0_CLKCFG_VAL 0
#define AHB_PRESCALAR 0
#define CLKOUT_SEL_VAL 0x0
#define CLKOUT_PRESCALAR 0x0

/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/

#define PLATFORM_MCUCONF

#endif /* MCUCONF_H */
