!if "$(BUILD)" == "release"
COMPILER_FLAGS = /O2 /GL
LINKER_FLAGS = /LTCG
!else if "$(BUILD)" == "debug"
COMPILER_FLAGS = /Zi
!else
COMPILER_FLAGS = /O1
!endif

build: ev.exe

rebuild: clean build

clean:
	-del ev *.a *.o *.d ev.exe *.lib *.obj *.pdb *.ilk t.* *.log

ev.exe: editor.obj foundation.obj file.obj application.obj input.obj console.obj graphics.obj main.obj
	cl /nologo /MT $** user32.lib ole32.lib dwrite.lib d2d1.lib windowscodecs.lib editor.res /link /out:$@ $(LINKER_FLAGS)

.cpp.obj:
	cl /c /nologo /std:c++latest /utf-8 /EHsc /MT /W3 /wd4244 /wd4267 /wd4723 /wd4838 /I. \
		/DGUI_MODE /D_UNICODE /DUNICODE /D_CRT_SECURE_NO_WARNINGS /D_CRT_NON_CONFORMING_SWPRINTFS $(COMPILER_FLAGS) $<

!if exists(depends.mk)
!include depends.mk
!endif
