CXX:=g++
CXXFLAGS:=-ansi -Wall

SRC:=./src
OBJ:=./obj

SRCS:=$(wildcard $(SRC)/*.c)
OBJS:=$(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
HDRS:=$(wildcard $(SRC)/*.h)

INCLUDE:=-I$(SRC)
APP:=caspian

$(OBJ)/%.o: $(SRC)/%.c $(HDRS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c     -o $@ $<

$(APP): $(OBJS) $(HDRS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -g -O0 -o $@ $(OBJS)

release:
	$(CXX) $(CXXFLAGS) $(INCLUDE)    -O2 -o $@ $(SRCS)

EXAMPLE:=./examples/hello_world.c
test: $(APP)
	./$(APP) $(EXAMPLE)

clean:
	rm -rf $(OBJ)/*.o $(APP)

valgrind:
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(APP) $(EXAMPLE)