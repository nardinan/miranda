objects = stream.obj.o json.obj.o resources.obj.o
name = miranda_io
debug = -Dd_miranda_debug=1
cc = clang -g $(debug)
cflags = -Wall -I. -fPIC -Wno-variadic-macros -Wno-pointer-arith -c
lflags = -Wall
libs = -L.. -L../.. -lpthread -lmiranda_ground -lmiranda_objects
executable = lib$(name).so

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) -shared $(libs)

stream.obj.o: stream.obj.c stream.obj.h ../string.obj.h
	$(cc) $(cflags) stream.obj.c

json.obj.o: json.obj.c json.obj.h stream.obj.h
	$(cc) $(cflags) json.obj.c

resources.obj.o: resources.obj.c resources.obj.h stream.obj.h
	$(cc) $(cflags) resources.obj.c

clean:
	rm -f *.o
	rm -f $(executable)
