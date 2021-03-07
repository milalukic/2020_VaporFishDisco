cmake_minimum_required(VERSION 3.17)
project(projekat_andjela_mila)

set(CMAKE_CXX_STANDARD 14)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(glad/)
add_executable(projekat_andjela_mila main.cpp)

target_link_libraries(projekat_andjela_mila glad glfw OpenGL::GL pthread X11 Xrandr Xi dl)
