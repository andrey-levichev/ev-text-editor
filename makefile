OS = $(shell uname)
BIN = bin/$(OS)/$(BUILD)/$(TARGET)

ifeq ($(OS), SunOS)
    CXX = CC
    COMPILER_FLAGS += -std=c++11 -xMMD
    LINKER_FLAGS += -std=c++11
    ifeq ($(BUILD), release)
        COMPILER_FLAGS += -fast -xtarget=generic -DDISABLE_ASSERT
        LINKER_FLAGS += -fast -xtarget=generic
    else ifeq ($(BUILD), debug)
        COMPILER_FLAGS += -g
    else
        COMPILER_FLAGS += -xO2
    endif
else ifeq ($(OS), AIX)
    CXX = xlclang++
    COMPILER_FLAGS += -MMD
    ifeq ($(BUILD), release)
        COMPILER_FLAGS += -Ofast -DDISABLE_ASSERT
        LINKER_FLAGS += -Ofast
    else ifeq ($(BUILD), debug)
        COMPILER_FLAGS += -g
    else
        COMPILER_FLAGS += -O
    endif
else ifeq ($(OS), Linux)
    COMPILER_FLAGS += -MMD -Wall -Wno-unused-but-set-variable -Wno-unused-variable
    ifeq ($(BUILD), release)
        COMPILER_FLAGS += -O3 -flto -DDISABLE_ASSERT
        LINKER_FLAGS += -O3 -flto
    else ifeq ($(BUILD), debug)
        COMPILER_FLAGS += -g
    else
        COMPILER_FLAGS += -Os
    endif
endif

ifeq ($(TARGET), test)
    EXE = $(BIN)/test
    OBJS = $(BIN)/test.o $(BIN)/foundation.o $(BIN)/file.o $(BIN)/input.o $(BIN)/console.o $(BIN)/main.o
else ifeq ($(TARGET), gui)
    COMPILER_FLAGS += -DGUI_MODE $(shell pkg-config --cflags gtk+-3.0)
    LINKER_FLAGS += -lrt $(shell pkg-config --libs gtk+-3.0)
    EXE = $(BIN)/ev
    OBJS = $(BIN)/editor.o $(BIN)/foundation.o $(BIN)/file.o $(BIN)/application.o \
        $(BIN)/input.o $(BIN)/console.o $(BIN)/graphics.o $(BIN)/main.o
else
    EXE = $(BIN)/ev
    OBJS = $(BIN)/editor.o $(BIN)/foundation.o $(BIN)/file.o $(BIN)/application.o \
        $(BIN)/input.o $(BIN)/console.o $(BIN)/main.o
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
	$(CXX) -c -I. -o $@ $< $(COMPILER_FLAGS)

-include $(BIN)/*.d
