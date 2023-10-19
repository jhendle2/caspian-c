CC:=gcc
# CFLAGS:=-ansi -Wall -DDISABLE_PREPROCESSOR
CFLAGS:=-std=c99 -Wall -DDISABLE_PREPROCESSOR

SRC:=./src
OBJ:=./obj

SRCS:=$(wildcard $(SRC)/*.c)
OBJS:=$(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
HDRS:=$(wildcard $(SRC)/*.h)

INCLUDE:=-I$(SRC)
APP:=caspian

$(OBJ)/%.o: $(SRC)/%.c $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDE) -c -g -Og -o $@ $<

$(APP): $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDE)    -g -Og -o $@ $(OBJS)

release:
	$(CC) $(CFLAGS) $(INCLUDE)       -O2 -o $@ $(SRCS)

# EXAMPLE:=./examples/simplest.c
EXAMPLE:=./examples/hello_world.c
test: $(APP)
	./$(APP) $(EXAMPLE)

clean:
	rm -rf $(OBJ)/*.o $(APP)

valgrind: $(APP)
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(APP) $(EXAMPLE)