cmake_minimum_required(VERSION 3.16)
project(LR3 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(lr3_core
    src/Bit.cpp
)

target_include_directories(lr3_core PUBLIC include)

add_executable(lr3_console src/main.cpp)
target_link_libraries(lr3_console PRIVATE lr3_core)

add_executable(lr3_tests tests/tests.cpp)
target_link_libraries(lr3_tests PRIVATE lr3_core)

add_executable(lr3_bench src/benchmark.cpp)
target_link_libraries(lr3_bench PRIVATE lr3_core)

find_package(PkgConfig)
if(PkgConfig_FOUND)
  pkg_check_modules(GTK3 gtk+-3.0)
  if(GTK3_FOUND)
    add_executable(lr3_gtk ui/gtk_main.cpp)
    target_include_directories(lr3_gtk PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(lr3_gtk PRIVATE lr3_core ${GTK3_LIBRARIES})
    target_compile_options(lr3_gtk PRIVATE ${GTK3_CFLAGS_OTHER})
  endif()
endif()
