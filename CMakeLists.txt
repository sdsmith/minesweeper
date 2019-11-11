# TODO(stewarts): preprocessor ddefins:
# - BUILD={debug,release}
# - TRAP_FP_FAULTS ?= false
# - ARCH={x86,x64}

cmake_minimum_required(VERSION 3.10.2)
project(minesweeper)

set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ..)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Dependencies
#
if (WIN32)
  list(APPEND CMAKE_PREFIX_PATH "D:/libraries/Development/glew-2.1.0")
endif()
find_package(GLEW REQUIRED)
if (GLEW_FOUND)
  include_directories(${GLEW_INCLUDE_DIRS})
  link_libraries(${GLEW_LIBRARIES})
endif()

find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
  include_directories(${OPENGL_INCLUDE_DIR})
  link_libraries(${OPENGL_LIBRARIES})
endif()

find_package(SDL2 REQUIRED)
if (SDL2_FOUND)
  include_directories(${SDL2_INCLUDE_DIRS})
  link_libraries(${SDL2_LIBRARIES})
endif()

find_package(GLM REQUIRED)
if (GLM_FOUND)
  include_directories(${GLM_INCLUDE_DIR})
endif()

# Source files
#
file(GLOB_RECURSE source_files "${source_dir}/*.cpp")

# Executable
#
add_executable(minesweeper ${source_files})

# Include paths
#
target_include_directories(minesweeper SYSTEM PUBLIC extern)
target_include_directories(minesweeper PUBLIC src)

# Compiler config
#
if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(GENERIC_CXX_FLAGS "-std=c++17 -g -Og -pthread -fPIC -Wall -Werror -Wextra -Weffc++ -Wfloat-equal -Werror-implicit-function-declaration -Wundef -Wpointer-arith -Wcast-align -Wstrict-overflow=4 -Wwrite-strings -Wno-unused-result")
  # Remove noisy warnings
  set(GENERIC_CXX_FLAGS "${GENERIC_CXX_FLAGS} -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code")

  set(CLANG_CXX_FLAGS "${GENERIC_CXX_FLAGS}")
  set(GCC_CXX_FLAGS "${GENERIC_CXX_FLAGS}")

  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_CXX_FLAGS}")
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CXX_FLAGS}")
  endif()
endif()
