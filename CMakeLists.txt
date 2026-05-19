cmake_minimum_required(VERSION 3.16)
project(LR2 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(lr2_core
    src/Bit.cpp
        include/DynamicArray.hpp
)

target_include_directories(lr2_core PUBLIC include)

add_executable(lr2_console src/main.cpp
        include/DynamicArray.hpp
        include/DynamicArray.hpp)
target_link_libraries(lr2_console PRIVATE lr2_core)

add_executable(lr2_tests tests/tests.cpp
        include/DynamicArray.hpp)
target_link_libraries(lr2_tests PRIVATE lr2_core)

find_package(PkgConfig)
if(PkgConfig_FOUND)
  pkg_check_modules(GTK3 gtk+-3.0)
  if(GTK3_FOUND)
    add_executable(lr2_gtk ui/gtk_main.cpp
            include/DynamicArray.hpp)
    target_include_directories(lr2_gtk PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(lr2_gtk PRIVATE lr2_core ${GTK3_LIBRARIES})
    target_compile_options(lr2_gtk PRIVATE ${GTK3_CFLAGS_OTHER})
  endif()
endif()
