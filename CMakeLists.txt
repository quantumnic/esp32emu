cmake_minimum_required(VERSION 3.14)
project(esp32emu VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Library ──────────────────────────────────────────────────────────
add_library(esp32emu
    src/globals.cpp
    src/webserver.cpp
    src/wificlient.cpp
    src/httpclient.cpp
)
target_include_directories(esp32emu PUBLIC include)

# ── Main entry point (for linking user sketches) ─────────────────────
add_library(esp32emu_main STATIC src/esp32emu_main.cpp)
target_link_libraries(esp32emu_main PUBLIC esp32emu)

# ── Tests ────────────────────────────────────────────────────────────
enable_testing()

file(GLOB TEST_SOURCES test/*.cpp)
foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} esp32emu)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# ── Examples ─────────────────────────────────────────────────────────
file(GLOB EXAMPLE_SOURCES examples/*.cpp)
foreach(ex_src ${EXAMPLE_SOURCES})
    get_filename_component(ex_name ${ex_src} NAME_WE)
    add_executable(${ex_name} ${ex_src} src/esp32emu_main.cpp)
    target_link_libraries(${ex_name} esp32emu)
endforeach()

# ── Install ──────────────────────────────────────────────────────────
install(TARGETS esp32emu ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include/esp32emu)
