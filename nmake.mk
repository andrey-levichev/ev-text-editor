.SUFFIXES: .h

BIN = bin\vcpp\$(BUILD)

!if "$(BUILD)" == "release"
COMPILER_FLAGS = /O2 /GL
LINKER_FLAGS = /LTCG
!else if "$(BUILD)" == "debug"
COMPILER_FLAGS = /Zi
!else
COMPILER_FLAGS = /O1
BIN = bin\vcpp
!endif

build: $(BIN) $(BIN)\ev.exe

$(BIN):
	mkdir $(BIN)

rebuild: clean build

clean:
	-del /s /q $(BIN) t.* *.log

$(BIN)\ev.exe: $(BIN)\editor.obj $(BIN)\foundation.obj $(BIN)\file.obj $(BIN)\application.obj $(BIN)\input.obj $(BIN)\console.obj $(BIN)\graphics.obj $(BIN)\main.obj
	cl /nologo /MT $** user32.lib ole32.lib dwrite.lib d2d1.lib windowscodecs.lib editor.res /link /out:$@ $(LINKER_FLAGS)

.h.cpp:
.cpp{$(BIN)\}.obj::
	cl /nologo /c /Fo:$(BIN)\ /MP16 /std:c++latest /utf-8 /EHsc /MT /W3 /wd4244 /wd4267 /wd4723 /wd4838 /I. \
		/DGUI_MODE /D_UNICODE /DUNICODE /D_CRT_SECURE_NO_WARNINGS /D_CRT_NON_CONFORMING_SWPRINTFS $(COMPILER_FLAGS) $<
