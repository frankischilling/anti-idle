CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lX11 -lm
TARGET = antiidle
SOURCE = src/antiidle.c
BUILD_DIR = build
BIN_DIR = bin
SRC_DIR = src

.PHONY: all clean install uninstall directories

all: directories $(BIN_DIR)/$(TARGET)

directories:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

$(BIN_DIR)/$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(TARGET) $(SOURCE) $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)

install: $(BIN_DIR)/$(TARGET)
	install -Dm755 $(BIN_DIR)/$(TARGET) $(DESTDIR)/usr/local/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)/usr/local/bin/$(TARGET)

