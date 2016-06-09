editor: editor.c
	gcc -std=c99 -g $^ -o $@

clean:
	-rm editor
