# Toolchain
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Hardware Floating Point Flags
FPU_FLAGS = -mfloat-abi=hard -mfpu=vfpv3

# Flags
CFLAGS = -mcpu=cortex-a8 $(FPU_FLAGS) -nostdlib -nostartfiles -ffreestanding \
         -I Lab001 -I Lab002/OS -I Lab002/Lib -I Lab002/program
ASFLAGS = -mcpu=cortex-a8 $(FPU_FLAGS)

LAB ?= 2

BUILD_DIR = build
TARGET = $(BUILD_DIR)/kernel

# ---- Lab selection ----

ifeq ($(LAB),1)
SRC_S = Lab001/root.s
OBJS = $(BUILD_DIR)/root.o $(BUILD_DIR)/hello.o
endif

ifeq ($(LAB),2)
SRC_S = Lab002/OS/root.s
OBJS = $(BUILD_DIR)/root.o \
       $(BUILD_DIR)/os.o \
       $(BUILD_DIR)/main.o \
       $(BUILD_DIR)/stdio.o \
       $(BUILD_DIR)/string.o
endif

LINKER = Lab002/OS/linker_beagle.ld

.PHONY: all beagle clean deploy

all: $(TARGET).bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ---- Compilation Rules ----
$(BUILD_DIR)/%.o: Lab001/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: Lab002/program/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/os.o: Lab002/OS/os.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/stdio.o: Lab002/Lib/stdio.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/string.o: Lab002/Lib/string.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/root.o: $(SRC_S) | $(BUILD_DIR)
	$(CC) $(ASFLAGS) -x assembler-with-cpp -c $< -o $@

# ---- Link (Added -lgcc to resolve math helpers) ----
$(TARGET).elf: $(OBJS) | $(BUILD_DIR)
	$(CC) -T $(LINKER) $(OBJS) -o $@ -nostdlib -lgcc

# ---- Binary ----
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

beagle: $(TARGET).bin
	cp $(TARGET).bin $(BUILD_DIR)/p1.bin
	cp $(TARGET).bin $(BUILD_DIR)/p2.bin
	./deploy_beagle.sh

clean:
	rm -rf $(BUILD_DIR)