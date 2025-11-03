CC      := cc
CFLAGS  := -Iinclude -Wall -Wextra -O2
LDFLAGS :=

SRC_DIR := src
BLD_DIR := build

SRC := $(SRC_DIR)/eds.c $(SRC_DIR)/game.c $(SRC_DIR)/screen.c $(SRC_DIR)/keyboard.c $(SRC_DIR)/ranking.c $(SRC_DIR)/main.c
OBJ := $(SRC:.c=.o)
BIN := $(BLD_DIR)/jogo_cli

all: cli

cli: $(BIN)

$(BIN): $(OBJ) | $(BLD_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BLD_DIR):
	@mkdir -p $(BLD_DIR)

run: cli
	@$(BIN)

clean:
	rm -rf $(BLD_DIR) $(OBJ)
