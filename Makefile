#
# Copyright Altera 2015
# All Rights Reserved.
#
ALT_DEVICE_FAMILY ?= soc_cv_av
SOCEDS_ROOT ?= $(SOCEDS_DEST_ROOT)
HWLIBS_ROOT = $(SOCEDS_ROOT)/ip/altera/hps/altera_hps/hwlib

HWLIBS_SRC  := 
EXAMPLE_SRC := test_trustzone.c main.c io.c
C_SRC       := $(EXAMPLE_SRC) $(HWLIBS_SRC)

LINKER_SCRIPT := cycloneV-dk-ram-modified.ld

MULTILIBFLAGS := -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=neon
CFLAGS  := -g -O0 -Wall -std=c99 $(MULTILIBFLAGS) -I$(HWLIBS_ROOT)/include -I$(HWLIBS_ROOT)/include/$(ALT_DEVICE_FAMILY) -D$(ALT_DEVICE_FAMILY)
LDFLAGS := -T$(LINKER_SCRIPT) $(MULTILIBFLAGS)
ASFLAGS := -march=armv7-a -mcpu=cortex-a9

CROSS_COMPILE := arm-altera-eabi-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)g++
OC := $(CROSS_COMPILE)objcopy
AS := $(CROSS_COMPILE)as
MKIMAGE := mkimage
RM := rm -rf
CP := cp -f

ELF ?= $(basename $(firstword $(C_SRC))).axf
OBJ := $(patsubst %.c,%.o,$(C_SRC))
BIN = $(basename $(firstword $(C_SRC))).bin
IMG = $(basename $(firstword $(C_SRC)))-mkimage.bin
ASF = utility.o

.PHONY: all
all:  $(ASF) $(BIN) $(ELF) $(IMG) $(SPL)

.PHONY: clean
clean:
	$(RM) io.o main.o test_trustzone.o utility.o test_trustzone.axf test_trustzone.bin test_trustzone-mkimage.bin

define SET_HWLIBS_DEPENDENCIES
$(1): $(2)
	$(CP) $(2) $(1)
endef

ALL_HWLIBS_SRC = $(wildcard $(HWLIBS_ROOT)/src/hwmgr/*.c) $(wildcard $(HWLIBS_ROOT)/src/hwmgr/$(ALT_DEVICE_FAMILY)/*.c)

$(foreach file,$(ALL_HWLIBS_SRC),$(eval $(call SET_HWLIBS_DEPENDENCIES,$(notdir $(file)),$(file))))

$(ASF):
	$(AS) $(ASFLAGS) -c utility.s -o utility.o

$(OBJ): %.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(ASF) $(OBJ) 
	$(LD) $(LDFLAGS) $(OBJ) utility.o -o $@
	
$(BIN): $(ELF)
	$(OC) -O binary $(ELF) $(BIN)
		
$(IMG): $(BIN)
	$(MKIMAGE) -A arm -T standalone -C none -a 0x100040 -e 0 -n "baremetal image" -d $(BIN) $(IMG)
