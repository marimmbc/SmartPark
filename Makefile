CC      := cc
CFLAGS  := -Iinclude -I/opt/homebrew/include -Wall -Wextra -O2
LDFLAGS := -L/opt/homebrew/lib -ltensorflow -ltensorflow_framework -lraylib -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -Wl,-rpath,/opt/homebrew/lib

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
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BLD_DIR):
	@mkdir -p $(BLD_DIR)

run: gui
	@$(BIN)

clean:
	rm -rf $(BLD_DIR) $(OBJ)
