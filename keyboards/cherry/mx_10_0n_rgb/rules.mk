SRC += config_led.c
SRC += led_matrix.c
SRC += backlight.c

OPT_DEFS += -DSN32_SPI_CLKDIV=11
OPT_DEFS += -O2

# MCU name
MCU_FAMILY = SN32
MCU_SERIES = SN32F240B
MCU_LDSCRIPT = SN32F240B
MCU_STARTUP = sn32f24xb
BOARD = SN_SN32F240B
MCU = cortex-m0
ARMV = 6
SN32_BOOTLOADER_ADDRESS = 0x1FFF0301
BOOTLOADER = sn32-dfu

# 0x210
# Build Options
#   comment out to disable the options.
#
LTO_ENABLE = no
MAGIC_ENABLE = yes
MAGIC_KEYCODE_ENABLE = yes
SLEEP_LED_ENABLE = no	    # Breathing sleep LED during USB suspend
SERIAL_LINK_ENABLE = no
WAIT_FOR_USB = no
CUSTOM_MATRIX = no
LED_MATRIX_ENABLE = no
LED_MATRIX_DRIVER = custom
RAW_ENABLE = yes
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
##EEPROM_DRIVER = wear_leveling
##WEAR_LEVELING_DRIVER = spi_flash
################################### EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN
#DEFERRED_EXEC_ENABLE = yes

### OPENRGB_ENABLE = yes

USE_CHIBIOS_CONTRIB = yes
