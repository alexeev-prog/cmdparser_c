# Compiler and flags
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror
BUILD_DIR = build
TARGET = $(BUILD_DIR)/cmdparsertest
INSTALL_PREFIX ?= /usr/local

# Project directories
SRC_DIR = src
LIB_DIR = lib

# Automatic source detection
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)  # Dependency files

.PHONY: all clean install uninstall

# Main build target
all: $(BUILD_DIR) $(TARGET)

# Link object files into executable
$(TARGET): $(OBJS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) -o $@ $^

# Compile each .c file to .o file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $< -> $@"
	$(CC) $(CFLAGS) -I$(LIB_DIR) -MMD -c -o $@ $<

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Install to system
install: $(TARGET)
	@echo "Installing to $(INSTALL_PREFIX)"
	# Install executable
	install -d $(INSTALL_PREFIX)/bin
	install -m 755 $(TARGET) $(INSTALL_PREFIX)/bin

	# Install header file
	install -d $(INSTALL_PREFIX)/lib
	install -m 644 $(LIB_DIR)/cmdparser.h $(INSTALL_PREFIX)/lib

# Uninstall from system
uninstall:
	@echo "Uninstalling from $(INSTALL_PREFIX)"
	rm -f $(INSTALL_PREFIX)/bin/$(TARGET)
	rm -f $(INSTALL_PREFIX)/lib/cmdparser.h

# Run tests (example)
compile: $(TARGET)

# Package for distribution
dist: clean
	mkdir -p dist
	tar czf dist/$(TARGET)-$(shell date +%Y%m%d).tar.gz Makefile $(SRC_DIR) $(LIB_DIR)
