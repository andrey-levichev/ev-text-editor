ev: editor.o libfoundation.a
	g++ -o $@ $^ -lrt

libfoundation.a: foundation.o file.o application.o input.o console.o graphics.o main.o
	ar rcs $@ $^

%.o: %.cpp
	g++ -c -std=gnu++14 -Wall -I. -Wno-unused-variable -Wno-unused-but-set-variable $< -MMD -MT depends.mk

-include depends.mk

clean:
	-rm ev *.a *.o *.d
