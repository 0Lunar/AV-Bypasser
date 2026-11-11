COMPILER = /usr/bin/nasm
LINKER = ld

COMPILER_FLAGS = -felf64

SOURCE_DIR = .
BUILD_DIR = build
TARGET = shellcode

SOURCE = shellcode
OBJECT = $(BUILD_DIR)/$(SOURCE).o

.PHONY: all clear

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECT)
	$(LINKER) -o $@ $^

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.s | $(BUILD_DIR)
	$(COMPILER) $(COMPILER_FLAGS) -o $@ $<

clear:
	rm -rf $(BUILD_DIR) $(TARGET)