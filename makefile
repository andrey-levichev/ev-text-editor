default: editor.linux

all:
	ssh nylxdev1 'cd ~/projects/editor; PATH=/opt/bb/bin:/opt/swt/install/gcc-4.9.2/bin:$$PATH make editor.linux'
	ssh njtsdev2 'cd ~/projects/editor; PATH=/opt/bb/bin:/home/aleviche/projects/solarisstudio12.5/bin:$$PATH make editor.solaris'
	ssh nyibldo5 'cd ~/projects/editor; PATH=/opt/bb/bin:/opt/bb/lib/xlc-13.01/opt/IBM/xlC/13.1.0/bin:$$PATH make editor.aix'

editor.linux: editor.c
	gcc -std=gnu11 -g $^ -o $@

editor.solaris: editor.c
	cc -g $^ -o $@

editor.aix: editor.c
	xlc -g $^ -o $@

clean:
	-rm editor.linux editor.solaris editor.aix

depl:
	cp editor.linux ~/bin/Linux/e
	cp editor.solaris ~/bin/SunOS/e
	cp editor.aix ~/bin/AIX/e

