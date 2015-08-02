library_path = /usr/lib
include_path = /usr/include
project = miranda
objects = endian.local.o exception.o hash.o list.o log.o memory.o string.local.o math.local.o types.o rs232.o console.o telnet.o huffman.o
name = miranda_ground
cc = gcc -g
cflags = -fPIC -Wall -Wno-variadic-macros -Wno-pointer-arith -c
lflags = -Wall
executable = lib$(name).so
folders = objects objects/io

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) -shared
	for current_dir in $(folders); do make -C $${current_dir}; done

install:
	for current_dir in $(folders); \
		do cp -f $${current_dir}/*.so $(library_path); done
	cp -f $(executable) $(library_path)
	mkdir -p $(include_path)/$(project)
	for current_dir in $(folders); \
		do mkdir -p $(include_path)/$(project)/$${current_dir} && cp -f $${current_dir}/*.h $(include_path)/$(project)/$${current_dir}; done
	cp -f *.h $(include_path)/$(project)

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

math.local.o: math.local.c math.local.h
	$(cc) $(cflags) math.local.c

types.o: types.c types.h
	$(cc) $(cflags) types.c

rs232.o: rs232.c rs232.h string.local.h
	$(cc) $(cflags) rs232.c

console.o: console.c console.h string.local.h memory.h
	$(cc) $(cflags) console.c

telnet.o: telnet.c telnet.h string.local.h memory.h
	$(cc) $(cflags) telnet.c

huffman.o: huffman.c huffman.h types.h memory.h
	$(cc) $(cflags) huffman.c

clean:
	rm -f *.o
	rm -f $(executable)
	for current_dir in $(folders); do make clean -C $${current_dir}; done
