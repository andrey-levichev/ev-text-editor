cmake_minimum_required(VERSION 4.0)

# cmake -G Ninja -S . -B build
# ninja -C build

project(ev)

add_executable(ev)
target_sources(ev PRIVATE application.cpp console.cpp editor.cpp
    file.cpp foundation.cpp input.cpp main.cpp graphics.cpp)
target_include_directories(ev PRIVATE .)

if(WIN32)
    target_link_libraries(ev PRIVATE ole32 dwrite d2d1 windowscodecs)
    target_compile_definitions(ev PRIVATE GUI_MODE UNICODE _UNICODE)
    set_property(TARGET ev PROPERTY WIN32_EXECUTABLE TRUE)
endif()
