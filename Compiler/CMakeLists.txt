cmake_minimum_required(VERSION 3.20)
project(LemurParser LANGUAGES CXX)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include")

# Compiler warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|AppleClang")
    add_compile_options(-Wall -Wextra -pedantic)
elseif (MSVC)
    add_compile_options(/W4)
endif()

# ---- Source Files ----
file(GLOB SUBDIRS RELATIVE ${SRC_DIR} ${SRC_DIR}/*)

set(LEMUR_CORE_SRC "")
foreach(SUBDIR ${SUBDIRS})
    if(IS_DIRECTORY "${SRC_DIR}/${SUBDIR}")
        file(GLOB_RECURSE DIR_SRC CONFIGURE_DEPENDS "${SRC_DIR}/${SUBDIR}/*.cpp")
        list(APPEND LEMUR_CORE_SRC ${DIR_SRC})
    endif()
endforeach()

add_library(lemur-core ${LEMUR_CORE_SRC})
target_include_directories(lemur-core PUBLIC ${INC_DIR})

# ---- Executable ----
add_executable(lemur
    ${SRC_DIR}/Run.cpp
)

target_link_libraries(lemur PRIVATE lemur-core)

set_target_properties(lemur PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/.."
)

# ---- GoogleTest Setup ----
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory(Tests)