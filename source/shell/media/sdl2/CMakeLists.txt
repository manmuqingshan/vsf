# CMakeLists head

if(VSF_USE_SDL2)
    target_include_directories(${VSF_TARGET_NAME} PUBLIC
        include
        include/SDL2
    )
    vsf_add_sources(
        vsf_sdl2.c
        vsf_sdl2_event.c
        vsf_sdl2_pixelformat.c
    )

    add_subdirectory(3rd-party)
endif()
