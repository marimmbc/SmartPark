CC       ?= cc
CFLAGS   ?= -Wall -Wextra -O2
CPPFLAGS ?=
LDFLAGS := -Llib -ltensorflow -ltensorflow_framework -Wl,-rpath,./lib
LDLIBS   ?=

RAYLIB_PREFIX     ?= /opt/homebrew
TENSORFLOW_PREFIX ?= /opt/homebrew

CPPFLAGS += -Iinclude -I$(RAYLIB_PREFIX)/include -I$(TENSORFLOW_PREFIX)/include
LDFLAGS  += -L$(RAYLIB_PREFIX)/lib -L$(TENSORFLOW_PREFIX)/lib -Wl,-rpath,$(TENSORFLOW_PREFIX)/lib
LDLIBS   += -lraylib -ltensorflow -ltensorflow_framework \
            -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL

SRC_DIR := src
BLD_DIR := build

SRC := \
$(SRC_DIR)/eds.c \
$(SRC_DIR)/game.c \
$(SRC_DIR)/ranking.c \
$(SRC_DIR)/sprites.c \
$(SRC_DIR)/gui.c \
$(SRC_DIR)/ia.c \
$(SRC_DIR)/main_gui.c

OBJ := $(SRC:.c=.o)
BIN := $(BLD_DIR)/jogo_gui

all: gui

gui: $(BIN)

$(BIN): $(OBJ) | $(BLD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS) $(LDLIBS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BLD_DIR):
	@mkdir -p $(BLD_DIR)

run: gui
	@$(BIN)

clean:
	rm -rf $(BLD_DIR) $(OBJ)
