.RECIPEPREFIX=>

editor: editor.c
> gcc -std=gnu11 -g $^ -o $@

clean:
> -rm editor
