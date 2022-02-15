GCCFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles
GCCPATH = aarch64-linux-gnu

SRC_DIR := src
INC_DIR := include
OBJ_DIR := build
BIN_DIR := bin

EXE := $(BIN_DIR)/kernel8.elf

SRC := $(wildcard $(SRC_DIR)/*.c)
INC := $(wildcard $(INC_DIR)/*.h)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

ASMSRC := $(wildcard $(SRC_DIR)/*.S)
ASMOBJ := $(ASMSRC:$(SRC_DIR)/%.S=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(EXE)

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

$(EXE): $(OBJ) $(ASMOBJ) | $(BIN_DIR)
	echo "		$(OBJ)"
	$(GCCPATH)-ld -nostdlib build/boot.o $(OBJ) -T linker.ld -o bin/kernel8.elf
	$(GCCPATH)-objcopy -O binary $(BIN_DIR)/kernel8.elf $(BIN_DIR)/kernel8.img

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	echo $<
	$(GCCPATH)-gcc $(GCCFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S | $(OBJ_DIR)
	echo $<
	$(GCCPATH)-gcc $(GCCFLAGS) -c $< -o $@


clean:
	rm -rfv $(BIN_DIR) $(OBJ_DIR)

debug:
	qemu-system-aarch64 -M raspi3b -serial stdio -kernel $(BIN_DIR)/kernel8.elf -s -S
	
run:
	qemu-system-aarch64 -M raspi3b -serial stdio -kernel $(BIN_DIR)/kernel8.elf