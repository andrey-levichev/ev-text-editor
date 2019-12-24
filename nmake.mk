.SUFFIXES: .h

BIN = bin\Windows

!if "$(BUILD)" == "release"

BIN = $(BIN)\$(BUILD)
COMPILER_FLAGS = /O2 /GL /DDISABLE_ASSERT
LINKER_FLAGS = /LTCG

!else if "$(BUILD)" == "debug"

BIN = $(BIN)\$(BUILD)
COMPILER_FLAGS = /Zi

!else

COMPILER_FLAGS = /O1

!endif

!if "$(TARGET)" == "test"

BIN = $(BIN)\$(TARGET)
EXE = $(BIN)\test.exe
OBJS = $(BIN)\test.obj $(BIN)\foundation.obj $(BIN)\file.obj $(BIN)\input.obj $(BIN)\console.obj $(BIN)\main.obj
LIBS = user32.lib ole32.lib

!else if "$(TARGET)" == "console"

COMPILER_FLAGS = $(COMPILER_FLAGS)
LIBS = user32.lib ole32.lib
BIN = $(BIN)\$(TARGET)
EXE = $(BIN)\ev.exe
OBJS = $(BIN)\editor.obj $(BIN)\foundation.obj $(BIN)\file.obj $(BIN)\application.obj \
	$(BIN)\input.obj $(BIN)\console.obj $(BIN)\main.obj

!else

COMPILER_FLAGS = $(COMPILER_FLAGS) /DGUI_MODE
LIBS = user32.lib ole32.lib dwrite.lib d2d1.lib windowscodecs.lib
EXE = $(BIN)\ev.exe
OBJS = $(BIN)\editor.obj $(BIN)\foundation.obj $(BIN)\file.obj $(BIN)\application.obj \
	$(BIN)\input.obj $(BIN)\console.obj $(BIN)\graphics.obj $(BIN)\main.obj $(BIN)\editor.res

!endif

build: $(BIN) $(EXE)

$(BIN):
	mkdir $(BIN)

rebuild: clean build

clean:
	-rmdir /s /q $(BIN)
	-del /q *.log

$(EXE): $(OBJS)
	cl /nologo /MT $** $(LIBS) /link /out:$@ $(LINKER_FLAGS)

.h.cpp:
.cpp{$(BIN)\}.obj::
	cl /nologo /c /Fo:$(BIN)\ /MP16 /std:c++latest /utf-8 /EHsc /MT /W3 /wd4244 /wd4267 /wd4723 /wd4838 /I. \
		/D_UNICODE /DUNICODE /D_CRT_SECURE_NO_WARNINGS /D_CRT_NON_CONFORMING_SWPRINTFS $(COMPILER_FLAGS) $<

.rc{$(BIN)\}.res:
	rc /nologo /fo$@ $<
