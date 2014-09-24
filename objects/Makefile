objects = object.o memory.obj.o string.obj.o
name = miranda_objects
cc = gcc -g
cflags = -Wall -I.. -fPIC -Wno-variadic-macros -Wno-gnu -Wno-pointer-arith -Wno-dollar-in-identifier-extension -std=c99 -c -pedantic
lflags = -Wall
libs = -L.. -lmiranda_ground
executable = lib$(name).so

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) -shared $(libs)

object.o: object.c object.h
	$(cc) $(cflags) object.c

memory.obj.o: memory.obj.c memory.obj.h object.h
	$(cc) $(cflags) memory.obj.c

string.obj.o: string.obj.c string.obj.h memory.obj.h
	$(cc) $(cflags) string.obj.c

clean:
	rm -f *.o
	rm -f $(executable)
