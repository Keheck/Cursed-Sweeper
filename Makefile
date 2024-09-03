# ==== DIRECTORIES ====
BUILD_DIR=build
BIN_DIR=$(BUILD_DIR)/bin
SRC_DIR=src
INCLUDE_DIR=include

# ==== FILES ====
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
OBJ_FILES=$(addprefix $(BUILD_DIR)/,$(notdir $(SRC_FILES:.c=.o)))
EXE_NAME=$(BIN_DIR)/cursed_sweeper

# ==== FLAGS ====
LDFLAGS=$(shell pkg-config --libs ncursesw)
COMMON_FLAGS=-O3 -Wall -Werror
CFLAGS=$(shell pkg-config --cflags ncursesw) -iquote include

# ==== RULES ====
$(EXE_NAME): $(OBJ_FILES)
	mkdir $(@D) -p
	$(CC) $^ $(COMMON_FLAGS) -o $@ $(LDFLAGS)

$(OBJ_FILES): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir $(@D) -p
	$(CC) $^ $(COMMON_FLAGS) $(CFLAGS) -c -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)