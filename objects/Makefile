objects = object.o memory.obj.o pool.obj.o string.obj.o array.obj.o stream.obj.o
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

pool.obj.o: pool.obj.c pool.obj.h memory.obj.h
	$(cc) $(cflags) pool.obj.c

string.obj.o: string.obj.c string.obj.h array.obj.h
	$(cc) $(cflags) string.obj.c

array.obj.o: array.obj.c array.obj.h memory.obj.h
	$(cc) $(cflags) array.obj.c

stream.obj.o: stream.obj.c stream.obj.h memory.obj.h
	$(cc) $(cflags) stream.obj.c

clean:
	rm -f *.o
	rm -f $(executable)
