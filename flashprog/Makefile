# ******************************************************************************
# ***** PROJECT-SPECIFIC SETTINGS                                          *****
# ******************************************************************************
# Project name
TARGET = flashprog

MCU   = atmega328pb		# Microcontroller
F_CPU = 16000000UL  	# Clock speed
BAUD  = 38400UL			# Download rate - can try 19200 or 38400

# Include file directories
LIBCOMMON = $(HOME)/avrlib/avr-common
LIBC = $(HOME)/avrlib/avr-c
LIBCPP = $(HOME)/avrlib/avr-c++

# Automatically include .c/.cpp files in the project's root directory.
# We can also specifically add files in other directories, such as a local
# library or our main libaries.
# In the latter case, it might be better sometimes to copy the library files
# to the local library subdir. This means the library files won't get updated
# automatically by later changes, but it also means those changes won't break
# anything for this program.
# C files
CSOURCES = $(wildcard *.c)   		# include all .c files in the current dir
# If there are .c sources in a local subdir or other location, add here:
#CSOURCES += $(wildcard locallib/*.c)
#CSOURCES += $(LIBDIR)/avr-c/example.c
CHEADERS = $(CSOURCES:.c=.h) 		# and their associated .h files

# C++ files
CPPSOURCES = $(wildcard *.cpp)
# Add sources from local lib subdir:
CPPSOURCES += $(wildcard lib/*.cpp)
# Add header files:
CPPHEADERS = $(CPPSOURCES:.cpp=.h)
CPPSOURCES += $(wildcard lib/*.h)

# ******************************************************************************
# ***** PROGRAMMER SETTINGS                                                *****
# ******************************************************************************
PROGRAMMER_TYPE = atmelice_isp
# extra arguments to avrdude: baud rate, chip type, -F flag, etc.
PROGRAMMER_ARGS = -B 125kHz

# ******************************************************************************
# ***** PROGRAM LOCATIONS                                                  *****
# ******************************************************************************
# May need to provide full paths if any of these are not in $PATH
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

# ******************************************************************************
# ***** MAKE SETTINGS & INSTRUCTIONS                                       *****
# ******************************************************************************
# The settings here will find all source and header files in the current
# directory and library locations.
#CSOURCES = $(wildcard *.c $(LIBDIR)/*.c $(LIBDIR)/avr-c/*.c $(LIBDIR)/avr-common/*.c $(LIBDIR)/avr-common/uproc/*.c)
OBJECTS = $(CSOURCES:.c=.o)
OBJECTS += $(CPPSOURCES:.cpp=.o)

## LDLIBS = -l$(LIBCOMMON) -l$(LIBCPP)

# Compilation options. NB: Added -I$(LIBDIR). If using more library paths,
# need to add them here.
CPPFLAGS = -Os -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I.
CPPFLAGS += -I$(LIBCOMMON) -I$(LIBCPP)
# -Os optimises for size & is generally the best option. Source file
# may also need to include:
#   #ifndef __OPTIMIZE__
#   #define __OPTIMIZE__
#   #endif
CFLAGS = -g -std=gnu99 -Wall
## Use short (8-bit) data types
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
## Splits up object files per function
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -I$(LIBC)
LDFLAGS = -DF_CPU=$(F_CPU) -Os -Wl,-Map,$(TARGET).map
## Optional, but often ends up with smaller code
LDFLAGS += -Wl,--gc-sections
## LDFLAGS += -L$(LIBCOMMON) -L$(LIBCPP)
## LDFLAGS += -l$(LIBCOMMON) -l$(LIBCPP)
## Relax shrinks code even more, but makes disassembly messy
## LDFLAGS += -Wl,--relax
## LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## for floating-point printf
## LDFLAGS += -Wl,-u,vfprintf -lprintf_min      ## for smaller printf
TARGET_ARCH = -mmcu=$(MCU)

# Pattern rules, to make .o files from .c files
%.o: %.c $(CHEADERS) Makefile
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;

%.o: %.cpp $(CPPHEADERS) Makefile
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;

$(TARGET).elf: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

%.hex: %.elf
	 $(OBJCOPY) -j .text -j .data -O ihex $< $@

%.eeprom: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%.lst: %.elf
	$(OBJDUMP) -S $< > $@

# Targets with no associated files
.PHONY: all disassemble disasm eeprom size clean squeaky_clean flash fuses

all: $(TARGET).hex

debug:
	@echo
	@echo "Source files:"
	@echo $(CSOURCES), $(CPPSOURCES)
	@echo "MCU, F_CPU, BAUD:"  $(MCU), $(F_CPU), $(BAUD)
	@echo "CFLAGS: " $(CFLAGS)
	@echo "CPPFLAGS: " $(CPPFLAGS)
	@echo

# Optionally create listing file from .elf
# This creates approximate assembly-language equivalent of your code.
# Useful for debugging time-sensitive bits,
# or making sure the compiler does what you want.
disassemble: $(TARGET).lst

disasm: disassemble

# Optionally show how big the resulting program is
size:  $(TARGET).elf
	$(AVRSIZE) -C --mcu=$(MCU) $(TARGET).elf

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).obj \
	$(TARGET).o $(TARGET).d $(TARGET).eep $(TARGET).lst \
	$(TARGET).lss $(TARGET).sym $(TARGET).map $(TARGET)~ \
	$(TARGET).eeprom

fullclean:
	rm -f *.elf *.hex *.obj *.o *.d *.eep *.lst *.lss *.sym *.map *~ *.eeprom lib/*.o

# ******************************************************************************
# ***** PROGRAMMER COMMANDS                                                *****
# ******************************************************************************
# These targets use Avrdude to burn the image file to the device.
flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

flash_eeprom: $(TARGET).eeprom
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U eeprom:w:$<

avrdude_terminal:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nt

avr-reset:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS)

# ******************************************************************************
# ***** FUSE SETTINGS                                                      *****
# ******************************************************************************

# Mega 48, 88, 168, 328 with 16MHz external clock, no div, preserve EEPROM
# Using these settings as my default values.
LFUSE = 0xFF
HFUSE = 0xD6
EFUSE = 0xFD

# Generic
FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

set_fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) \
	           $(PROGRAMMER_ARGS) $(FUSE_STRING)
show_fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nv

# Mega 48, 88, 168, 328 using internal clock, divided by 8
set_fuses_int_slow: LFUSE = 0x62
set_fuses_int_slow: HFUSE = 0xDF
set_fuses_int_slow: EFUSE = 0x00
set_fuses_int_slow: set_fuses

# Mega 48, 88, 168, 328 using internal clock, no clock div
set_fuses_int_fast: LFUSE = 0xE2
set_fuses_int_fast: HFUSE = 0xDF
set_fuses_int_fast: EFUSE = 0x00
set_fuses_int_fast: set_fuses

# Mega 48, 88, 168, 328 with 16MHz external clock, no div, preserve EEPROM
set_fuses_ext_clk: LFUSE = 0xFF
set_fuses_ext_clk: HFUSE = 0xD6
set_fuses_ext_clk: EFUSE = 0xFD
set_fuses_ext_clk: set_fuses
