objects = object.o memory.obj.o mutex.obj.o emitter.obj.o pool.obj.o string.obj.o array.obj.o
name = miranda_objects
debug = -Dd_miranda_debug=1
cc = gcc -g $(debug)
cflags = -Wall -I. -fPIC -Wno-variadic-macros -Wno-pointer-arith -c
lflags = -Wall
libs = -L.. -lpthread -lmiranda_ground
executable = lib$(name).so

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) -shared $(libs)

object.o: object.c object.h
	$(cc) $(cflags) object.c

memory.obj.o: memory.obj.c memory.obj.h object.h
	$(cc) $(cflags) memory.obj.c

mutex.obj.o: mutex.obj.c mutex.obj.h object.h
	$(cc) $(cflags) mutex.obj.c

emitter.obj.o: emitter.obj.c emitter.obj.h object.h
	$(cc) $(cflags) emitter.obj.c

pool.obj.o: pool.obj.c pool.obj.h memory.obj.h mutex.obj.h
	$(cc) $(cflags) pool.obj.c

string.obj.o: string.obj.c string.obj.h array.obj.h
	$(cc) $(cflags) string.obj.c

array.obj.o: array.obj.c array.obj.h memory.obj.h mutex.obj.h
	$(cc) $(cflags) array.obj.c

clean:
	rm -f *.o
	rm -f $(executable)
