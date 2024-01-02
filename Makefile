# Update the source and library directories
SRC_DIR = src
OUTPUT_DIR = output

# List source files with paths
SRC_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/sdl.c $(SRC_DIR)/map.c $(SRC_DIR)/pacman.c $(SRC_DIR)/ui.c $(SRC_DIR)/ghosts.c 

# Generate object file names with output directory
OBJFILES = $(patsubst $(SRC_DIR)/%.c,$(OUTPUT_DIR)/%.o,$(SRC_FILES))

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -fsanitize=address -g -I$(SRC_DIR)/headers
LDFLAGS = -lSDL2 -lSDL2_ttf -lm

# Rule to create .o files in the output directory
$(OUTPUT_DIR)/%.o: $(SRC_DIR)/%.c | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o main $(LDFLAGS)

dep:
	$(CC) $(CFLAGS) -MM $(SRC_FILES) >dep.list

-include dep.list

clean:
	rm -rf $(OUTPUT_DIR) main dep.list

# Create output directory if it doesn't exist
$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)