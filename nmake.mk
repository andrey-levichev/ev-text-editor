ev.exe: editor.obj foundation.obj file.obj application.obj input.obj console.obj graphics.obj main.obj
	cl /nologo /MT $** user32.lib ole32.lib dwrite.lib d2d1.lib windowscodecs.lib editor.res \
		/link /out:$@

!include depends.mk

.cpp.obj:
	cl /c /nologo /std:c++latest /utf-8 /EHsc /MT /W3 /wd4244 /wd4267 /wd4723 /wd4838 /I. \
		/DGUI_MODE /D_UNICODE /DUNICODE /D_CRT_SECURE_NO_WARNINGS /D_CRT_NON_CONFORMING_SWPRINTFS $<

clean:
	-del ev.exe *.lib *.obj *.pdb *.ilk
