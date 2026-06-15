CC = gcc
CFLAGS = -Wall -Wextra -O2 `pkg-config --cflags sdl2 SDL2_image SDL2_mixer`
LDFLAGS = `pkg-config --libs sdl2 SDL2_image SDL2_mixer` -ljson-c -lm

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/channels/channel.c \
          $(SRC_DIR)/ui/ui.c \
          $(SRC_DIR)/ui/banner.c \
          $(SRC_DIR)/theme/theme.c \
          $(SRC_DIR)/audio/audio.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TARGET = $(BIN_DIR)/wii-frontend

all: $(TARGET)

$(TARGET): $(OBJECTS)
    @mkdir -p $(BIN_DIR)
    $(CC) $(OBJECTS) $(LDFLAGS) -o $@
    @echo "✓ Build completato: $(TARGET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
    @mkdir -p $(dir $@)
    $(CC) $(CFLAGS) -c $< -o $@
    @echo "✓ Compilato: $<"

clean:
    rm -rf $(BUILD_DIR) $(BIN_DIR)
    @echo "✓ Pulizia completata"

.PHONY: all clean
