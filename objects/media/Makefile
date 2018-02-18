objects = primitives.o environment.obj.o drawable.obj.o eventable.obj.o morphable.obj.o controllable.obj.o bitmap.obj.o particle.obj.o animation.obj.o \
		  transition.obj.o track.obj.o entity.obj.o lights.obj.o media_factory.obj.o
name = miranda_media
debug = -Dd_miranda_debug=1
cc = gcc -g $(debug)
cflags = -Wall -I. `sdl2-config --cflags` -fPIC -Wno-variadic-macros -Wno-pointer-arith -Wno-pointer-to-int-cast -c
lflags = -Wall
libs = -L.. -L../geometry -L../io -L../.. -lpthread -lmiranda_ground -lmiranda_objects -lmiranda_io -lmiranda_geometry \
	   `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer
executable = lib$(name).so

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) -shared $(libs)

primitives.o: primitives.c primitives.h
	$(cc) $(cflags) primitives.c

environment.obj.o: environment.obj.c environment.obj.h primitives.h drawable.obj.h
	$(cc) $(cflags) environment.obj.c

drawable.obj.o: drawable.obj.c drawable.obj.h ../geometry/point.obj.h ../geometry/square.obj.h
	$(cc) $(cflags) drawable.obj.c

eventable.obj.o: eventable.obj.c eventable.obj.h ../memory.obj.h ../mutex.obj.h
	$(cc) $(cflags) eventable.obj.c

morphable.obj.o: morphable.obj.c morphable.obj.h eventable.obj.h drawable.obj.h
	$(cc) $(cflags) morphable.obj.c

controllable.obj.o: controllable.obj.c controllable.obj.h eventable.obj.h
	$(cc) $(cflags) controllable.obj.c

bitmap.obj.o: bitmap.obj.c bitmap.obj.h environment.obj.h ../io/stream.obj.h
	$(cc) $(cflags) bitmap.obj.c

particle.obj.o: particle.obj.c particle.obj.h environment.obj.h
	$(cc) $(cflags) particle.obj.c

animation.obj.o: animation.obj.c animation.obj.h environment.obj.h
	$(cc) $(cflags) animation.obj.c

transition.obj.o: transition.obj.c transition.obj.h environment.obj.h
	$(cc) $(cflags) transition.obj.c

track.obj.o: track.obj.c track.obj.h bitmap.obj.h
	$(cc) $(cflags) track.obj.c

entity.obj.o: entity.obj.c entity.obj.h drawable.obj.h
	$(cc) $(cflags) entity.obj.c

lights.obj.o: lights.obj.c lights.obj.h drawable.obj.h
	$(cc) $(cflags) lights.obj.c

media_factory.obj.o: media_factory.obj.c media_factory.obj.h ../io/resources.obj.h ../io/json.obj.h ../io/lisp.obj.h bitmap.obj.h particle.obj.h \
	animation.obj.h transition.obj.h track.obj.h
	$(cc) $(cflags) media_factory.obj.c

clean:
	rm -f *.o
	rm -f $(executable)
