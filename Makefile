CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -Iinclude

TARGET = log_server
SRC = $(wildcard src/*.c)
BIN_DIR = bin
OBJ = $(patsubst src/%.c,$(BIN_DIR)/%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%.o: src/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

re: clean all

.PHONY: all clean re
