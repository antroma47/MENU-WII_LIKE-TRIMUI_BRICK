# Makefile - Wii Frontend for TrimUI Brick

CC         := gcc
CFLAGS     := -Wall -Wextra -O2 -std=c99 -fPIC
LDFLAGS    := -lSDL2 -lSDL2_image -lSDL2_mixer -lm
SRCDIR     := src
OBJDIR     := build
BINDIR     := bin

# Sorgenti
SOURCES := $(SRCDIR)/main.c \
           $(SRCDIR)/channels/channel.c \
           $(SRCDIR)/ui/ui.c \
           $(SRCDIR)/ui/banner.c \
           $(SRCDIR)/theme/theme.c \
           $(SRCDIR)/audio/audio.c

OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET  := $(BINDIR)/wii-frontend

# ===== TARGETS =====

.PHONY: all clean help

all: $(TARGET)
    @echo "✓ Build completato: $(TARGET)"

$(TARGET): $(OBJECTS) | $(BINDIR)
    @mkdir -p $(dir $@)
    $(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
    @echo "✓ Linking completato"

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
    @mkdir -p $(dir $@)
    $(CC) $(CFLAGS) -c $< -o $@
    @echo "✓ Compilato: $<"

$(OBJDIR):
    @mkdir -p $@

$(BINDIR):
    @mkdir -p $@

clean:
    @rm -rf $(OBJDIR) $(BINDIR)
    @echo "✓ Pulizia completata"

help:
    @echo "Comandi disponibili:"
    @echo "  make        - Compila il progetto"
    @echo "  make clean  - Rimuove i file compilati"
    @echo "  make help   - Mostra questo messaggio"
