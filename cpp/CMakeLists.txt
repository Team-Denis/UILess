# Minimum CMake version
cmake_minimum_required(VERSION 3.27)

# Project name and C++ standard
project(UILessShell)
set(CMAKE_CXX_STANDARD 23)

# Compiler warnings and optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -O2 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op \
                     -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align \
                     -Wnull-dereference -Wold-style-cast -Wuseless-cast \
                     -Wdouble-promotion -Wzero-as-null-pointer-constant -Wstrict-null-sentinel \
                     -Wswitch-enum -Wswitch-default -Wunreachable-code -Wundef")

# Sanitizers (optional, uncomment if needed)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")

# Include directories
include_directories(include)

# Source files
file(GLOB SRC_FILES "src/*.cpp")

find_package(raylib REQUIRED)
find_package(nlohmann_json REQUIRED)

# Executable target
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib PRIVATE nlohmann_json::nlohmann_json)

# Option to build tests
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    # Enable testing
    enable_testing()

    include_directories(include)

    # Add test executable
    add_executable(pipeline_tests tests/pipelineTests.cpp src/commandHandler.cpp src/pipelineRunner.cpp src/result.cpp src/commandSpec.cpp src/tokenizer.cpp)
    add_executable(thread_tests tests/threadingTests.cpp src/commandHandler.cpp src/pipelineRunner.cpp src/result.cpp src/cmdThread.cpp)

    # Link libraries to test executable
    target_link_libraries(pipeline_tests PRIVATE nlohmann_json::nlohmann_json)
    target_link_libraries(thread_tests PRIVATE nlohmann_json::nlohmann_json)

    # Register the test
    add_test(NAME RunTests COMMAND pipeline_tests)
    add_test(NAME RunThreadTests COMMAND thread_tests)
endif()