SRC_DIR = src
OBJ_DIR = obj

CC = clang
CFLAGS = -O3 -std=c23 -D_XOPEN_SOURCE=500
TARGET = mpt

LDFLAGS = -O3

SRCS = $(shell find src -type f -name '*.c')
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $< -o $@ $(CFLAGS) -c

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
