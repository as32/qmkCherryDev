##MCU_LDSCRIPT = SN32F260

TOP_SYMBOLS = 20



SRC += custom_drivers/no_idle_thread.c

####LDFLAGS += --specs=nano.specs
## USE_LINK_GC = yes

### OPT_DEFS = -Os

### EXTRAFLAGS += -flto

# process stack size of 0x1c0 crashes during SEND_STRING
USE_EXCEPTIONS_STACKSIZE = 0xD0
# # 0x180 140 f0 d0    80gg a0gg b0gg b8gg c0 ok
USE_PROCESS_STACKSIZE = 0x1E0

SPACE_CADET_ENABLE = no
GRAVE_ESC_ENABLE = no 
MAGIC_ENABLE = no

AVR_USE_MINIMAL_PRINTF = yes
