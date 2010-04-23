foo: foo.c
	gcc -o foo foo.c `pkg-config --cflags --libs sdl cairo`
