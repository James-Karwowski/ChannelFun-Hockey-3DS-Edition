#---------------------------------------------------------------------------------
# Minimal Makefile (hardcoded include/lib paths)
#---------------------------------------------------------------------------------

# Project settings
TARGET    := 3DSProject
SOURCES   := source
INCLUDES  := include

# Toolchain (compiler from devkitARM)
DEVKITPRO := C:/devkitPro
DEVKITARM := $(DEVKITPRO)/devkitARM
CC        := $(DEVKITARM)/bin/arm-none-eabi-gcc

# Compiler/Linker flags
ARCH      := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CFLAGS    := -g -Wall -O2 -mword-relocations -ffunction-sections $(ARCH)
CFLAGS    += -Isource -Iinclude -I$(DEVKITPRO)/libctru/include
LDFLAGS   := -L$(DEVKITPRO)/libctru/lib -specs=3dsx.specs -g $(ARCH)

LIBS := -lcitro2d -lcitro3d -lctru -lm

#---------------------------------------------------------------------------------
# Build rules
#---------------------------------------------------------------------------------
OBJS := $(SOURCES)/main.o

.PHONY: all clean

all: $(TARGET).3dsx

$(SOURCES)/main.o: $(SOURCES)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(TARGET).3dsx: $(TARGET).elf
	3dsxtool $< $@

clean:
	@echo "Cleaning..."
	@rm -f $(SOURCES)/*.o $(TARGET).elf $(TARGET).3dsx
