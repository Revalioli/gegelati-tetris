cmake_minimum_required(VERSION 3.19)

# *******************************************
# ************* CMake Content ***************
# *******************************************
# This CMake create a workspace containing the following projects
#
# Programs
#  - gegelati-tetris

set(PROJECT_NAME gegelati_tetris)
project(${PROJECT_NAME})

# Disable C and C++ compiler extensions.
# C/CXX_EXTENSIONS are ON by default to allow the compilers to use extended
# variants of the C/CXX language.
# However, this could expose cross-platform bugs in user code or in the headers
# of third-party dependencies and thus it is strongly suggested to turn
# extensions off.
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
    # Link with pthread
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

    # Debug or release
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        MESSAGE("Generate Debug project")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -pg -Wall")
    else()
        MESSAGE("Generate Release project")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall")
    endif()
    #add libmath during non visual studio builds
    set(CMAKE_EXTRA_LIB m)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Add definitions for testing purposes
if(${TESTING})
    MESSAGE("Testing mode")
    # add_definitions(-DNB_GENERATIONS=10 -DNO_CONSOLE_CONTROL)
endif()

# *******************************************
# *********** GEGELATI LIBRARY **************
# *******************************************

if(WIN32)
    set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    # find the gegelatilib-x.y.z folder in the lib directory.
    file(GLOB GEGELATI_ROOT_DIR "${LIBS_DIR}/gegelatilib-[\\.|0-9]*")
    set(ENV{GEGELATI_DIR} ${GEGELATI_ROOT_DIR})
endif()
find_package(GEGELATI)


if (WIN32)
    file(GLOB
            GEGELATI_DLL
            ${GEGELATI_ROOT_DIR}/bin/*.dll
            )

    MESSAGE("Copy GEGELATI DLLs into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    if(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    else()
        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
    endif()
endif()

# *******************************************
# ************** Executable  ****************
# *******************************************

include_directories(${GEGELATI_INCLUDE_DIRS})

add_executable(tetris_game src/tetris_game.cpp src/Tetris.cpp src/Tetris.h src/Render.cpp src/Render.h)
target_link_libraries(tetris_game ${GEGELATI_LIBRARIES} sfml-graphics sfml-window sfml-system)
target_compile_definitions(tetris_game PRIVATE ROOT_DIR="${CMAKE_SOURCE_DIR}")


add_executable(tetris src/main.cpp src/Tetris.cpp src/Tetris.h src/Render.cpp src/Render.h src/instructions.cpp src/instructions.h)
target_link_libraries(tetris ${GEGELATI_LIBRARIES} sfml-graphics sfml-window sfml-system)
target_compile_definitions(tetris PRIVATE ROOT_DIR="${CMAKE_SOURCE_DIR}")

add_executable(tetris_no_replay src/main.cpp src/Tetris.cpp src/Tetris.h src/Render.cpp src/Render.h src/instructions.cpp src/instructions.h)
target_link_libraries(tetris_no_replay ${GEGELATI_LIBRARIES} sfml-graphics sfml-window sfml-system)
target_compile_definitions(tetris_no_replay PRIVATE ROOT_DIR="${CMAKE_SOURCE_DIR}" NO_REPLAY)


add_executable(tetrisInference src/mainInference.cpp src/Tetris.cpp src/Tetris.h src/Render.cpp src/Render.h src/instructions.cpp src/instructions.h)
target_link_libraries(tetrisInference ${GEGELATI_LIBRARIES} sfml-graphics sfml-window sfml-system)
target_compile_definitions(tetrisInference PRIVATE ROOT_DIR="${CMAKE_SOURCE_DIR}")


add_custom_target(clean_dot rm ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/out_*.dot)