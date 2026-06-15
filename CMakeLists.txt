cmake_minimum_required(VERSION 3.16)
project(wii-menu-trimui C)

set(CMAKE_C_STANDARD 11)

# Ottimizzazioni per ARM Cortex-A53
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -march=armv8-a -mtune=cortex-a53")

# Trova SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

include_directories(
    ${SDL2_INCLUDE_DIRS}
    src
)

# Tutti i sorgenti
set(SOURCES
    src/main.c
    src/ui/channels.c
    src/ui/banner.c
    src/ui/animations.c
    src/ui/widgets.c
    src/input/input.c
    src/audio/audio.c
    src/theme/theme.c
)

add_executable(wii-menu-trimui ${SOURCES})

target_link_libraries(wii-menu-trimui
    ${SDL2_LIBRARIES}
    SDL2_image
    SDL2_ttf
    SDL2_mixer
    m
)

# Output nella cartella bin
set_target_properties(wii-menu-trimui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
