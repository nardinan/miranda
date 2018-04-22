cmake_minimum_required(VERSION 3.9)
execute_process(COMMAND sdl2-config --libs 2>/dev/null OUTPUT_VARIABLE STATUS_SDL)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-Dd_miranda_debug)
endif()
add_library(miranda_ground SHARED
        console.c
        console.h
        endian.local.c
        endian.local.h
        exception.c
        exception.h
        ground.h
        hash.c
        hash.h
        huffman.c
        huffman.h
        list.c
        list.h
        log.c
        log.h
        logs.h
        math.local.c
        math.local.h
        memory.c
        memory.h
        rs232.c
        rs232.h
        string.local.c
        string.local.h
        telnet.c
        telnet.h
        types.c
        types.h)
string(CONCAT miranda_ground_properties
        "console.h;endian.local.h;exception.h;ground.h;hash.h;"
        "huffman.h;list.h;log.h;logs.h;math.local.h;memory.h;rs232.h;string.local.h;telnet.h;types.h")
set_target_properties(miranda_ground PROPERTIES PUBLIC_HEADER "${miranda_ground_properties}")
INSTALL(TARGETS miranda_ground
        LIBRARY DESTINATION /usr/local/lib
        PUBLIC_HEADER DESTINATION /usr/local/include/miranda
        )

add_library(miranda_objects SHARED
        objects/array.obj.c
        objects/array.obj.h
        objects/emitter.obj.c
        objects/emitter.obj.h
        objects/memory.obj.c
        objects/memory.obj.h
        objects/mutex.obj.c
        objects/mutex.obj.h
        objects/object.c
        objects/object.h
        objects/objects.h
        objects/pool.obj.c
        objects/pool.obj.h
        objects/runnable.obj.c
        objects/runnable.obj.h
        objects/string.obj.c
        objects/string.obj.h
        objects/map.obj.c
        objects/map.obj.h
        objects/timer.obj.c
        objects/timer.obj.h
        objects/transformations.obj.c
        objects/transformations.obj.h)
string(CONCAT miranda_objects_properties
        "objects/array.obj.h;objects/emitter.obj.h;"
        "objects/memory.obj.h;objects/mutex.obj.h;objects/object.h;objects/objects.h;objects/pool.obj.h;objects/runnable.obj.h;"
        "objects/string.obj.h;objects/map.obj.h;objects/transformations.obj.h")
set_target_properties(miranda_objects PROPERTIES PUBLIC_HEADER "${miranda_objects_properties}")
INSTALL(TARGETS miranda_objects
        LIBRARY DESTINATION /usr/local/lib
        PUBLIC_HEADER DESTINATION /usr/local/include/miranda/objects
        )

add_library(miranda_geometry SHARED
        objects/geometry/geometry.h
        objects/geometry/line.obj.c
        objects/geometry/line.obj.h
        objects/geometry/point.obj.c
        objects/geometry/point.obj.h
        objects/geometry/square.obj.c
        objects/geometry/square.obj.h)
string(CONCAT miranda_geometry_properties
        "objects/geometry/geometry.h;"
        "objects/geometry/line.obj.h;objects/geometry/point.obj.h;objects/geometry/square.obj.h")
set_target_properties(miranda_geometry PROPERTIES PUBLIC_HEADER "${miranda_geometry_properties}")
INSTALL(TARGETS miranda_geometry
        LIBRARY DESTINATION /usr/local/lib
        PUBLIC_HEADER DESTINATION /usr/local/include/miranda/objects/geometry
        )

add_library(miranda_io SHARED
        objects/io/io.h
        objects/io/json.obj.c
        objects/io/json.obj.h
        objects/io/lisp.obj.c
        objects/io/lisp.obj.h
        objects/io/resources.obj.c
        objects/io/resources.obj.h
        objects/io/stream.obj.c
        objects/io/stream.obj.h)
string(CONCAT miranda_io_properties
        "objects/io/io.h;objects/io/json.obj.h;"
        "objects/io/lisp.obj.h;objects/io/resources.obj.h;objects/io/stream.obj.h")
set_target_properties(miranda_io PROPERTIES PUBLIC_HEADER "${miranda_io_properties}")
INSTALL(TARGETS miranda_io
        LIBRARY DESTINATION /usr/local/lib
        PUBLIC_HEADER DESTINATION /usr/local/include/miranda/objects/io
        )

add_library(miranda_network SHARED
        objects/network/network.h
        objects/network/payload.obj.c
        objects/network/payload.obj.h)
string(CONCAT miranda_network_properties
        "objects/network/network.h;objects/network/payload.obj.h")
set_target_properties(miranda_network PROPERTIES PUBLIC_HEADER "${miranda_network_properties}")
INSTALL(TARGETS miranda_network
        LIBRARY DESTINATION /usr/local/lib
        PUBLIC_HEADER DESTINATION /usr/local/include/miranda/objects/network
        )

#SDL dependent code
if(NOT (STATUS_SDL STREQUAL ""))
    add_library(miranda_media SHARED
            objects/media/animation.obj.c
            objects/media/animation.obj.h
            objects/media/bitmap.obj.c
            objects/media/bitmap.obj.h
            objects/media/controllable.obj.c
            objects/media/controllable.obj.h
            objects/media/drawable.obj.c
            objects/media/drawable.obj.h
            objects/media/entity.obj.c
            objects/media/entity.obj.h
            objects/media/environment.obj.c
            objects/media/environment.obj.h
            objects/media/eventable.obj.c
            objects/media/eventable.obj.h
            objects/media/lights.obj.c
            objects/media/lights.obj.h
            objects/media/media.h
            objects/media/media_factory.obj.c
            objects/media/media_factory.obj.h
            objects/media/morphable.obj.c
            objects/media/morphable.obj.h
            objects/media/particle.obj.c
            objects/media/particle.obj.h
            objects/media/primitives.c
            objects/media/primitives.h
            objects/media/track.obj.c
            objects/media/track.obj.h
            objects/media/transition.obj.c
            objects/media/transition.obj.h
            objects/media/camera.obj.c
            objects/media/camera.obj.h
            objects/media/camera_controller.obj.c
            objects/media/camera_controller.obj.h
            objects/media/illuminable_bitmap.obj.c
            objects/media/illuminable_bitmap.obj.h)
    string(CONCAT miranda_media_properties
            "objects/media/animation.obj.h;objects/media/bitmap.obj.h;"
            "objects/media/controllable.obj.h;objects/media/drawable.obj.h;objects/media/entity.obj.h;"
            "objects/media/environment.obj.h;objects/media/eventable.obj.h;objects/media/lights.obj.h;"
            "objects/media/media.h;objects/media/media_factory.obj.h;objects/media/morphable.obj.h;objects/media/particle.obj.h;"
            "objects/media/primitives.h;objects/media/track.obj.h;objects/media/transition.obj.h;objects/media/illuminable_bitmap.obj.h")
    set_target_properties(miranda_media PROPERTIES PUBLIC_HEADER "${miranda_media_properties}")
    INSTALL(TARGETS miranda_media
            LIBRARY DESTINATION /usr/local/lib
            PUBLIC_HEADER DESTINATION /usr/local/include/miranda/objects/media
            )

    add_library(miranda_cameras SHARED
            objects/media/cameras/cameras.h
            objects/media/cameras/drawable_chaser.obj.c
            objects/media/cameras/drawable_chaser.obj.h
            objects/media/cameras/limiter.obj.c
            objects/media/cameras/limiter.obj.h)
    string(CONCAT miranda_cameras_properties
            "objects/media/cameras/cameras.h;"
            "objects/media/cameras/drawable_chaser.obj.h;objects/media/cameras/limiter.obj.h")
    set_target_properties(miranda_cameras PROPERTIES PUBLIC_HEADER "${miranda_cameras_properties}")
    INSTALL(TARGETS miranda_cameras
            LIBRARY DESTINATION /usr/local/lib
            PUBLIC_HEADER DESTINATION /usr/local/include/miranda/objects/media/cameras
            )

    add_library(miranda_ui SHARED
            objects/media/ui/button.obj.c
            objects/media/ui/button.obj.h
            objects/media/ui/checkbox.obj.c
            objects/media/ui/checkbox.obj.h
            objects/media/ui/container.obj.c
            objects/media/ui/container.obj.h
            objects/media/ui/field.obj.c
            objects/media/ui/field.obj.h
            objects/media/ui/fonts.obj.c
            objects/media/ui/fonts.obj.h
            objects/media/ui/label.obj.c
            objects/media/ui/label.obj.h
            objects/media/ui/list.obj.c
            objects/media/ui/list.obj.h
            objects/media/ui/scroll.obj.c
            objects/media/ui/scroll.obj.h
            objects/media/ui/ui.h
            objects/media/ui/ui_factory.obj.c
            objects/media/ui/ui_factory.obj.h
            objects/media/ui/uiable.obj.c
            objects/media/ui/uiable.obj.h)
    string(CONCAT miranda_ui_properties
            "objects/media/ui/button.obj.h;"
            "objects/media/ui/checkbox.obj.h;objects/media/ui/container.obj.h;objects/media/ui/field.obj.h;"
            "objects/media/ui/fonts.obj.h;objects/media/ui/label.obj.h;objects/media/ui/list.obj.h;objects/media/ui/scroll.obj.h;"
            "objects/media/ui/ui.h;objects/media/ui/ui_factory.obj.h;objects/media/ui/uiable.obj.h")
    set_target_properties(miranda_ui PROPERTIES PUBLIC_HEADER "${miranda_ui_properties}")
    INSTALL(TARGETS miranda_ui
            LIBRARY DESTINATION /usr/local/lib
            PUBLIC_HEADER DESTINATION /usr/local/include/miranda/objects/media/ui
            )
endif()

target_link_libraries(miranda_objects miranda_ground)
target_link_libraries(miranda_geometry miranda_objects)
target_link_libraries(miranda_io miranda_objects)
target_link_libraries(miranda_network miranda_objects)

# SDL dependent code
if(NOT (STATUS_SDL STREQUAL ""))
    target_link_libraries(miranda_media miranda_objects miranda_geometry miranda_io miranda_ground)
    target_link_libraries(miranda_cameras miranda_media)
    target_link_libraries(miranda_ui miranda_media)

    INCLUDE(FindPkgConfig)
    PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
    PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
    PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)
    PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer)

    INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS})

    TARGET_LINK_LIBRARIES(miranda_media ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2MIXER_LIBRARIES})
    TARGET_LINK_LIBRARIES(miranda_ui ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2MIXER_LIBRARIES})
endif()
