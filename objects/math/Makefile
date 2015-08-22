objects = point.obj.o line.obj.o
name = miranda_math
debug = -Dd_miranda_debug=1
cc = gcc -g $(debug)
cflags = -Wall -I. -fPIC -Wno-variadic-macros -Wno-pointer-arith -c
lflags = -Wall
libs = -L.. -L../.. -lpthread -lmiranda_ground -lmiranda_objects
executable = lib$(name).so

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) -shared $(libs)

point.obj.o: point.obj.c point.obj.h ../memory.obj.h ../mutex.obj.h ../../math.local.h
	$(cc) $(cflags) point.obj.c

line.obj.o: line.obj.c line.obj.h point.obj.h
	$(cc) $(cflags) line.obj.c

clean:
	rm -f *.o
	rm -f $(executable)
