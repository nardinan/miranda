objects = environment.obj.o drawable.obj.o eventable.obj.o bitmap.obj.o particle.obj.o animation.obj.o
name = miranda_graphic
debug = -Dd_miranda_debug=1
cc = gcc -g $(debug)
cflags = -Wall -I. -fPIC -Wno-variadic-macros -Wno-pointer-arith -Wno-pointer-to-int-cast -c
lflags = -Wall
libs = -L.. -L../math -L../io -L../.. -lpthread -lmiranda_ground -lmiranda_objects -lmiranda_io -lmiranda_math -lSDL2 -lSDL2_image -lSDL2_ttf
executable = lib$(name).so

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) -shared $(libs)

environment.obj.o: environment.obj.c environment.obj.h drawable.obj.h
	$(cc) $(cflags) environment.obj.c

drawable.obj.o: drawable.obj.c drawable.obj.h ../math/point.obj.h
	$(cc) $(cflags) drawable.obj.c

eventable.obj.o: eventable.obj.c eventable.obj.h ../memory.obj.h ../mutex.obj.h
	$(cc) $(cflags) eventable.obj.c

bitmap.obj.o: bitmap.obj.c bitmap.obj.h environment.obj.h ../io/stream.obj.h
	$(cc) $(cflags) bitmap.obj.c

particle.obj.o: particle.obj.c particle.obj.h environment.obj.h
	$(cc) $(cflags) particle.obj.c

animation.obj.o: animation.obj.c animation.obj.h environment.obj.h
	$(cc) $(cflags) animation.obj.c

clean:
	rm -f *.o
	rm -f $(executable)
