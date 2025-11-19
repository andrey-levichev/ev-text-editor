cmake_minimum_required(VERSION 4.0)

# cmake -G Ninja -S . -B build
# ninja -C build

project(ev)

add_executable(ev)
target_sources(ev PRIVATE application.cpp console.cpp editor.cpp
file.cpp foundation.cpp input.cpp main.cpp graphics.cpp)
target_include_directories(ev PRIVATE .)
target_link_libraries(ev PRIVATE ole32 dwrite d2d1 windowscodecs)
target_compile_options(ev PRIVATE -O2 -DGUI_MODE -municode -mwindows)
target_link_options(ev PRIVATE -municode -mwindows)
