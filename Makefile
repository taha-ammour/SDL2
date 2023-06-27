# Compiler and linker flags
CFLAGS = -I src/include -L src/lib
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_TTF -lSDL2_image -lSDL2_mixer -lgmp

# Specify build directory
BUILD_DIR = build

# Default target
all: $(BUILD_DIR)/main

# Build target for app
$(BUILD_DIR)/main: main.c auth.c shop.c | $(BUILD_DIR)
	gcc $(CFLAGS) -o $@ $^ $(LIBS)

# Build target for debugging
debug: CFLAGS += -g
debug: $(BUILD_DIR)/main

# Target to create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Target to clean up
clean:
	rm -rf $(BUILD_DIR)
