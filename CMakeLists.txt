cmake_minimum_required(VERSION 3.16)
project(overlite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

add_executable(overlite src/main.cpp src/ui.cpp src/logic.cpp)
target_link_libraries(overlite PRIVATE SDL3_ttf::SDL3_ttf SDL3::SDL3)

if(MSVC)
  target_compile_options(overlite PRIVATE /W4 /utf-8)
else()
  target_compile_options(overlite PRIVATE -Wall -Wextra)
endif()

# Pruebas de la lógica (no necesitan SDL):  cmake -B build -DOVERLITE_TESTS=ON
option(OVERLITE_TESTS "Compilar las pruebas de la lógica" OFF)
if(OVERLITE_TESTS)
  add_executable(test_logic tests/test_logic.cpp src/logic.cpp)
  target_include_directories(test_logic PRIVATE src)
  enable_testing()
  add_test(NAME logic COMMAND test_logic)
endif()
