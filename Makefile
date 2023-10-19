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

SIMPLEST:=./examples/simplest.c
HELLO_WORLD:=./examples/hello_world.c
VARS:=./examples/vars.c
EXAMPLE:=$(HELLO_WORLD)
test: $(APP)
	./$(APP) $(EXAMPLE)
simplest: $(APP)
	./$(APP) $(SIMPLEST)
hello-world: $(APP)
	./$(APP) $(HELLO_WORLD)
vars: $(APP)
	./$(APP) $(VARS)

clean:
	rm -rf $(OBJ)/*.o $(APP)

valgrind: $(APP)
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(APP) $(EXAMPLE)
valgrind-simplest: $(APP)
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(APP) $(SIMPLEST)
valgrind-hello-world: $(APP)
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(APP) $(HELLO_WORLD)
valgrind-vars: $(APP)
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(APP) $(VARS)