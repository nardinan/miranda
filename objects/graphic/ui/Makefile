objects = fonts.obj.o label.obj.o
name = miranda_ui
debug = -Dd_miranda_debug=1
cc = gcc -g $(debug)
cflags = -Wall -I. -fPIC -Wno-variadic-macros -Wno-pointer-arith -Wno-pointer-to-int-cast -c
lflags = -Wall
libs = -L.. -L../.. -L../../.. -lpthread -lmiranda_ground -lmiranda_objects -lmiranda_graphic -lSDL2 -lSDL2_image -lSDL2_ttf
executable = lib$(name).so

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) -shared $(libs)

fonts.obj.o: fonts.obj.c fonts.obj.h ../bitmap.obj.h
	$(cc) $(cflags) fonts.obj.c

label.obj.o: label.obj.c label.obj.h fonts.obj.h
	$(cc) $(cflags) label.obj.c

clean:
	rm -f *.o
	rm -f $(executable)
