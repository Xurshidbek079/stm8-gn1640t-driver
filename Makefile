# Compiler and tools
CC = sdcc
FLASH = stm8flash
PROGRAMMER = stlinkv2  # Change if using different programmer

# Target MCU
MCU = stm8s003f3      # Change to your specific MCU

# Directories
SRC_DIR = Core/src
INC_DIR = Core/inc
SPL_SRC = SPL/src
SPL_INC = SPL/inc
BUILD_DIR = Build

# Source files
CORE_SRCS = $(wildcard $(SRC_DIR)/*.c)
SPL_SRCS = $(wildcard $(SPL_SRC)/*.c)
ALL_SRCS = $(CORE_SRCS) $(SPL_SRCS)

# Object files in build directory
CORE_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/core/%.rel,$(CORE_SRCS))
SPL_OBJS = $(patsubst $(SPL_SRC)/%.c,$(BUILD_DIR)/spl/%.rel,$(SPL_SRCS))
ALL_OBJS = $(CORE_OBJS) $(SPL_OBJS)

# Include paths
INCLUDES = -I$(INC_DIR) -I$(SPL_INC)

# Compiler flags
CFLAGS = -mstm8 --std-sdcc99 --opt-code-size
CFLAGS += $(INCLUDES)
CFLAGS += -DSTM8S103  # Change according to your MCU

# Linker flags
LDFLAGS = -mstm8 -lstm8 --out-fmt-ihx

# Platform-specific command shortcuts (Windows vs Unix)
ifeq ($(OS),Windows_NT)
  MKDIR_CORE = @cmd /C "if not exist $(subst /,\\,$(BUILD_DIR))\core mkdir $(subst /,\\,$(BUILD_DIR))\core"
  MKDIR_SPL  = @cmd /C "if not exist $(subst /,\\,$(BUILD_DIR))\spl mkdir $(subst /,\\,$(BUILD_DIR))\spl"
  RM_CMD     = @cmd /C "if exist $(subst /,\\,$(BUILD_DIR)) rmdir /S /Q $(subst /,\\,$(BUILD_DIR))"
else
  MKDIR_CORE = @mkdir -p $(BUILD_DIR)/core
  MKDIR_SPL  = @mkdir -p $(BUILD_DIR)/spl
  RM_CMD     = @rm -rf $(BUILD_DIR)
endif

# Target
TARGET = main

.PHONY: all clean flash debug

all: $(BUILD_DIR)/$(TARGET).ihx

# Create build directories
$(BUILD_DIR)/core:
	$(MKDIR_CORE)

$(BUILD_DIR)/spl:
	$(MKDIR_SPL)

# Compile Core sources
$(BUILD_DIR)/core/%.rel: $(SRC_DIR)/%.c | $(BUILD_DIR)/core
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Compile SPL sources
$(BUILD_DIR)/spl/%.rel: $(SPL_SRC)/%.c | $(BUILD_DIR)/spl
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(BUILD_DIR)/$(TARGET).ihx: $(ALL_OBJS)
	@echo "Linking..."
	$(CC) $(LDFLAGS) $(ALL_OBJS) -o $@
	@echo "Build complete: $@"
	@echo "Size information:"
ifeq ($(OS),Windows_NT)
	@powershell -NoProfile -Command "Get-Item '$(subst /,\\,$@)' | Select-Object Name,Length"
else
	@ls -lh $@
endif

# Flash to MCU
flash: $(BUILD_DIR)/$(TARGET).ihx
	@echo "Flashing to MCU..."
	$(FLASH) -c $(PROGRAMMER) -p $(MCU) -w $<

# Clean build files
clean:
	@echo "Cleaning build files..."
	$(RM_CMD)
	@echo "Clean complete"

# For debugging
debug: $(BUILD_DIR)/$(TARGET).ihx
	@echo "Starting GDB server..."
	@echo "In another terminal, run: stm8-gdb $(BUILD_DIR)/$(TARGET).ihx"

# Show configuration
info:
	@echo "=== Build Configuration ==="
	@echo "MCU: $(MCU)"
	@echo "Programmer: $(PROGRAMMER)"
	@echo "Core sources: $(CORE_SRCS)"
	@echo "SPL sources: $(SPL_SRCS)"
	@echo "Build directory: $(BUILD_DIR)"
	@echo "=========================="