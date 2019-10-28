COMPILER_FLAGS = -c -std=gnu++14 -Wall -I. -MMD -MT depends.mk -Wno-unused-variable

ifeq ($(BUILD), release)
COMPILER_FLAGS += -O3 -flto -DDISABLE_ASSERT
else ifeq ($(BUILD), debug)
COMPILER_FLAGS += -g
else
COMPILER_FLAGS += -Os
endif

ifeq ($(CXX), g++)
COMPILER_FLAGS += -Wno-unused-but-set-variable
endif

build: ev

rebuild: clean build

clean:
	-rm ev *.a *.o *.d ev.exe *.lib *.obj *.pdb *.ilk t.* *.log

ev: editor.o foundation.o file.o application.o input.o console.o graphics.o main.o
	$(CXX) -o $@ $^ -lrt

%.o: %.cpp
	$(CXX) $(COMPILER_FLAGS) $<

-include depends.mk
