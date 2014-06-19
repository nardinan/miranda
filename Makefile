objects = endian.local.o exception.o hash.o list.o log.o memory.o string.local.o types.o rs232.o
name = miranda_ground
cc = gcc -g
cflags = -Wall -Wno-variadic-macros -Wno-pointer-arith -c
lflags = -Wall
executable = lib$(name).so

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) -shared

endian.local.o: endian.local.c endian.local.h types.h
	$(cc) $(cflags) endian.local.c

exception.o: exception.c exception.h
	$(cc) $(cflags) exception.c

hash.o: hash.c hash.h string.local.h memory.h
	$(cc) $(cflags) hash.c

list.o: list.c list.h string.local.h
	$(cc) $(cflags) list.c

log.o: log.c log.h
	$(cc) $(cflags) log.c

memory.o: memory.c memory.h types.h list.h
	$(cc) $(cflags) memory.c

string.local.o: string.local.c string.local.h types.h logs.h
	$(cc) $(cflags) string.local.c

types.o: types.c types.h
	$(cc) $(cflags) types.c

rs232.o: rs232.c rs232.h string.local.h
	$(cc) $(cflags) rs232.c

clean:
	rm -f *.o
	rm -f $(executable)
