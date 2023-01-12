# sdl2 linking variables
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2 SDL2_image SDL2_gfx)

# your app
file(GLOB SRC "./*.cpp" "libs/*.cpp")
add_executable(tetris ${SRC})
target_include_directories(tetris PUBLIC ${SDL2_INCLUDE_DIRS})
target_compile_options(tetris PUBLIC ${SDL2_CFLAGS_OTHER})
target_link_libraries(tetris ${SDL2_LIBRARIES})