CC      := cc
CFLAGS  := -Iinclude -Wall -Wextra -O2
LDFLAGS :=

SRC_DIR := src
BLD_DIR := build

SRC_BASE    := $(SRC_DIR)/eds.c $(SRC_DIR)/game.c $(SRC_DIR)/main.c
SRC_SCREEN  := $(SRC_DIR)/screen.c
SRC_KB      := $(SRC_DIR)/keyboard.c
SRC_GUI     := $(SRC_DIR)/gui.c $(SRC_DIR)/main_gui.c

RAYLIB_LDLIBS := -lraylib -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL

BIN_CLI_BASE   := $(BLD_DIR)/jogo_cli
BIN_CLI_SCREEN := $(BLD_DIR)/jogo_cli_screen
BIN_CLI_KB     := $(BLD_DIR)/jogo_cli_kb
BIN_CLI_FULL   := $(BLD_DIR)/jogo_cli_full
BIN_GUI        := $(BLD_DIR)/jogo_gui

OBJ_BASE    := $(SRC_BASE:.c=.o)
OBJ_SCREEN  := $(SRC_SCREEN:.c=.o)
OBJ_KB      := $(SRC_KB:.c=.o)
OBJ_GUI     := $(SRC_GUI:.c=.o)

all: cli

cli: $(BIN_CLI_BASE)
	@echo "✅ CLI básico pronto: $(BIN_CLI_BASE)"

cli_screen: $(BIN_CLI_SCREEN)
	@echo "✅ CLI com screen pronto: $(BIN_CLI_SCREEN)"

cli_kb: $(BIN_CLI_KB)
	@echo "✅ CLI com keyboard pronto: $(BIN_CLI_KB)"

cli_full: $(BIN_CLI_FULL)
	@echo "✅ CLI completo pronto: $(BIN_CLI_FULL)"

gui: $(BIN_GUI)
	@echo "✅ GUI pronta: $(BIN_GUI)"

$(BIN_CLI_BASE): $(OBJ_BASE) | $(BLD_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ_BASE) $(LDFLAGS)

$(BIN_CLI_SCREEN): $(OBJ_BASE) $(OBJ_SCREEN) | $(BLD_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ_BASE) $(OBJ_SCREEN) $(LDFLAGS)

$(BIN_CLI_KB): $(OBJ_BASE) $(OBJ_KB) | $(BLD_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ_BASE) $(OBJ_KB) $(LDFLAGS)

$(BIN_CLI_FULL): $(OBJ_BASE) $(OBJ_SCREEN) $(OBJ_KB) | $(BLD_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ_BASE) $(OBJ_SCREEN) $(OBJ_KB) $(LDFLAGS)

$(BIN_GUI): $(OBJ_BASE) $(OBJ_SCREEN) $(OBJ_GUI) | $(BLD_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ_BASE) $(OBJ_SCREEN) $(OBJ_GUI) $(RAYLIB_LDLIBS) $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: cli
	@$(BIN_CLI_BASE)

run_screen: cli_screen
	@$(BIN_CLI_SCREEN)

run_kb: cli_kb
	@$(BIN_CLI_KB)

run_full: cli_full
	@$(BIN_CLI_FULL)

run_gui: gui
	@$(BIN_GUI)

$(BLD_DIR):
	@mkdir -p $(BLD_DIR)

clean:
	rm -rf $(BLD_DIR) $(OBJ_BASE) $(OBJ_SCREEN) $(OBJ_KB) $(OBJ_GUI)
