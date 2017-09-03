objects = payload.obj.o
name = miranda_network
debug = -Dd_miranda_debug=1
cc = clang -g $(debug)
cflags = -Wall -I. -fPIC -Wno-variadic-macros -Wno-pointer-arith -c
lflags = -Wall
libs = -L.. -L../.. -lpthread -lmiranda_ground -lmiranda_objects
executable = lib$(name).so

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) -shared $(libs)

payload.obj.o: payload.obj.c ../memory.obj.h ../object.h
	$(cc) $(cflags) payload.obj.c

clean:
	rm -f *.o
	rm -f $(executable)
