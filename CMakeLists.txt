cmake_minimum_required(VERSION 3.19)

project("Chip8_Emulator"
        VERSION 1.0.0
        LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(SRC_FILES
    src/main.cpp
    src/chip8.cpp
    src/log.cpp
)

add_executable(emu ${SRC_FILES})

target_include_directories(emu
    PRIVATE include/
)

find_package(SDL3 REQUIRED CONFIG REQUIRED)
target_link_libraries(emu
    PRIVATE SDL3::SDL3
)


# Testing 

# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   # Specify the commit you depend on and update it regularly.
#   URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
# )
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)
#
# add_executable(test_chip8 tests/test_chip8.cpp)
#
# target_link_libraries(test_chip8
#     PRIVATE gtest_main
#     PRIVATE SDL3::SDL3
# )
#
# add_test(NAME test_chip8 COMMAND test_chip8)
