default: eve.linux

all:
	ssh nylxdev2 'cd ~/projects/eve; PATH=/opt/bb/bin:/opt/swt/install/gcc-4.9.2/bin:$$PATH make eve.linux'
	ssh njtsdev2 'cd ~/projects/eve; PATH=/opt/bb/bin:/home/aleviche/projects/solarisstudio12.5/bin:$$PATH make eve.solaris'
	ssh nyibldo5 'cd ~/projects/eve; PATH=/opt/bb/bin:/opt/bb/lib/xlc-13.01/opt/IBM/xlC/13.1.0/bin:$$PATH make eve.aix'

eve.linux: eve.c
	gcc -std=gnu11 -O2 $^ -o $@

eve.solaris: eve.c
	cc -O2 $^ -o $@

eve.aix: eve.c
	xlc -O2 $^ -o $@

clean:
	-rm eve.linux eve.solaris eve.aix

depl:
	cp eve.linux ~/bin/Linux/eve
	cp eve.solaris ~/bin/SunOS/eve
	cp eve.aix ~/bin/AIX/eve

