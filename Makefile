CC = gcc
CFLAGS =
LDFLAGS = -lSDL2 -lm
SRC_DIR = src
BUILD_DIR = build
OBJ_FILES = $(BUILD_DIR)/main.o $(BUILD_DIR)/raycaster.o $(BUILD_DIR)/map.o $(BUILD_DIR)/vectors.o
TARGET = main

# target

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJ_FILES)

# object files

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# directories

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# clean

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
