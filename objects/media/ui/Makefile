objects = fonts.obj.o uiable.obj.o label.obj.o field.obj.o button.obj.o scroll.obj.o
name = miranda_ui
debug = -Dd_miranda_debug=1
cc = gcc -g $(debug)
cflags = -Wall -I. -fPIC -Wno-variadic-macros -Wno-pointer-arith -Wno-pointer-to-int-cast -c
lflags = -Wall
libs = -L.. -L../.. -L../../geometry -L../../.. -lpthread -lmiranda_ground -lmiranda_objects -lmiranda_geometry -lmiranda_media -lSDL2 -lSDL2_image -lSDL2_ttf
executable = lib$(name).so

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) -shared $(libs)

fonts.obj.o: fonts.obj.c fonts.obj.h ../bitmap.obj.h
	$(cc) $(cflags) fonts.obj.c

uiable.obj.o: uiable.obj.c uiable.obj.h ../environment.obj.h ../../emitter.obj.h
	$(cc) $(cflags) uiable.obj.c

label.obj.o: label.obj.c label.obj.h fonts.obj.h
	$(cc) $(cflags) label.obj.c

field.obj.o: field.obj.c field.obj.h label.obj.h
	$(cc) $(cflags) field.obj.c

button.obj.o: button.obj.c button.obj.h label.obj.h
	$(cc) $(cflags) button.obj.c

scroll.obj.o: scroll.obj.c scroll.obj.h uiable.obj.h
	$(cc) $(cflags) scroll.obj.c

clean:
	rm -f *.o
	rm -f $(executable)
