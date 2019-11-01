COMPILER_FLAGS = -c -std=gnu++14 -Wall -I. -MMD -Wno-unused-variable
BIN = bin/$(CXX)/$(BUILD)

ifeq ($(BUILD), release)
COMPILER_FLAGS += -O3 -flto -DDISABLE_ASSERT
else ifeq ($(BUILD), debug)
COMPILER_FLAGS += -g
else
COMPILER_FLAGS += -Os
BIN = bin/$(CXX)
endif

ifeq ($(CXX), g++)
COMPILER_FLAGS += -Wno-unused-but-set-variable
LINKER_FLAGS += -lrt
endif

build: $(BIN) $(BIN)/ev

$(BIN):
	mkdir -p $(BIN)

rebuild: clean build

clean:
	-rm -rf bin t.* *.log

$(BIN)/ev: $(BIN)/editor.o $(BIN)/foundation.o $(BIN)/file.o $(BIN)/application.o $(BIN)/input.o $(BIN)/console.o $(BIN)/graphics.o $(BIN)/main.o
	$(CXX) -o $@ $^ $(LINKER_FLAGS)

$(BIN)/%.o: %.cpp
	$(CXX) -o $@ $< $(COMPILER_FLAGS)

-include $(BIN)/*.d
