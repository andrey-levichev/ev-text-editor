OS=$(shell uname)
BIN = bin/$(OS)

ifeq ($(BUILD), release)

BIN := $(BIN)/$(BUILD)
COMPILER_FLAGS += -O3 -flto -DDISABLE_ASSERT

else ifeq ($(BUILD), debug)

BIN := $(BIN)/$(BUILD)
COMPILER_FLAGS += -g

else

COMPILER_FLAGS += -Os

endif

ifeq ($(OS), Linux)
COMPILER_FLAGS += -Wno-unused-but-set-variable
endif

ifeq ($(TARGET), test)

BIN := $(BIN)/$(TARGET)
EXE = $(BIN)/test
OBJS = $(BIN)/test.o $(BIN)/foundation.o $(BIN)/file.o $(BIN)/input.o $(BIN)/console.o $(BIN)/main.o

else ifeq ($(TARGET), console)

BIN := $(BIN)/$(TARGET)
EXE = $(BIN)/ev
OBJS = $(BIN)/editor.o $(BIN)/foundation.o $(BIN)/file.o $(BIN)/application.o \
	$(BIN)/input.o $(BIN)/console.o $(BIN)/graphics.o $(BIN)/main.o

else

COMPILER_FLAGS += -DGUI_MODE $(shell pkg-config --cflags gtk+-3.0)
LINKER_FLAGS += -lrt $(shell pkg-config --libs gtk+-3.0)
EXE = $(BIN)/ev
OBJS = $(BIN)/editor.o $(BIN)/foundation.o $(BIN)/file.o $(BIN)/application.o \
	$(BIN)/input.o $(BIN)/console.o $(BIN)/graphics.o $(BIN)/main.o

endif

build: $(BIN) $(EXE)

$(BIN):
	mkdir -p $(BIN)

rebuild: clean build

clean:
	-rm -rf $(BIN) *.log

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(LINKER_FLAGS)

$(BIN)/%.o: %.cpp
	$(CXX) -c -std=gnu++14 -Wall -I. -MMD -Wno-unused-variable -o $@ $< $(COMPILER_FLAGS)

-include $(BIN)/*.d
