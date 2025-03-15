

TARGET_ASMFLAGS+= -f elf
TARGET_CFLAGS+= -ffreestanding -nostdlib -Iinclude
TARGET_CXXFLAGS+= -ffreestanding -nostdlib -Iinclude -fno-rtti
TARGET_LIBS+= -lgcc
TARGET_LINKFLAGS+= -T linker.ld -nostdlib

HEADERS_C=$(shell find . -type f -name "*.h")
SOURCES_C=$(shell find . -type f -name "*.c")

HEADERS_CXX=$(shell find . -type f -name "*.hpp")
SOURCES_CXX=$(shell find . -type f -name "*.cpp")

HEADERS_ASM=$(shell find . -type f -name "*.inc")
SOURCES_ASM=$(shell find . -type f -name "*.asm")

OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/kernel/c/%.obj, $(SOURCES_C))
OBJECTS_CXX=$(patsubst %.cpp, $(BUILD_DIR)/kernel/cpp/%.obj, $(SOURCES_CXX))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/kernel/asm/%.obj, $(SOURCES_ASM))

.PHONY: all kernel clean always

all: kernel

kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: $(OBJECTS_ASM) $(OBJECTS_C) $(OBJECTS_CXX)
	$(TARGET_LD) $(TARGET_LINKFLAGS) -Wl,-Map=$(BUILD_DIR)/kernel.map -o $@ $^ $(TARGET_LIBS)

$(BUILD_DIR)/kernel/c/%.obj: %.c $(HEADERS_C)
	@mkdir -p $(@D)
	@$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

$(BUILD_DIR)/kernel/cpp/%.obj: %.cpp $(HEADERS_CXX)
	@mkdir -p $(@D)
	@$(TARGET_CXX) $(TARGET_CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/kernel/asm/%.obj: %.asm $(HEADERS_ASM)
	@mkdir -p $(@D)
	@$(TARGET_ASM) $(TARGET_ASMFLAGS) -o $@ $<

always:
	mkdir -p $(BUILD_DIR)/kernel/c
	mkdir -p $(BUILD_DIR)/kernel/cpp
	mkdir -p $(BUILD_DIR)/kernel/asm

clean:
	rm -f $(BUILD_DIR)/kernel.bin
	