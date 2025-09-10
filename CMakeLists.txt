cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+

# Set the C standard to C11, as it's one of the most recent stable standards, widely supported.
# NOTE: The project is using C, but some dependencies (like tracy) require C++.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (MSVC)
    # For MSVC, set the runtime library to MultiThreaded for release builds
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:DEBUG>:DEBUG>")
else()
    # For other compilers, use the default C runtime library
    set(CMAKE_C_STANDARD_LIBRARIES "")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Debug build should NOT optimize and should include debug symbols
# Would add -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls as well, but tricky using on Windows with msys2/MINGW, so gdb has to be used instead
set(CMAKE_C_FLAGS_DEBUG "-O0 -DDEBUG -g -gcodeview -gdwarf-4 -Wno-declaration-after-statement")

# Release build should optimize and NOT include debug symbols, disable asserts
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")


if (COMPAT_MODE)
    # in case the CPU doesn't support AVX2, use AVX1
    # For Intel HD Graphics 4000 (minimum support level), drop it down to OpenGL 3.3, if the compatibility flag is set
    # For other GPUs, use OpenGL 4.3
    set(EXECUTABLE_NAME "ca_renderer_compat")
    set(CMAKE_C_ADDITIONAL_FLAGS "${COMMON_FLAGS_COMPAT}")
    set(GRAPHICS_LEVEL ${GRAPHICS_COMPAT_LEVEL} CACHE STRING "Graphics API Level" FORCE)
else()
    set(EXECUTABLE_NAME "ca_renderer")
    set(CMAKE_C_ADDITIONAL_FLAGS "${COMMON_FLAGS}")
    set(GRAPHICS_LEVEL ${GRAPHICS_LEVEL} CACHE STRING "Graphics API Level" FORCE)
endif()

# Set compiler flags for the project
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything -Wno-missing-prototypes -Wno-strict-prototypes ${CMAKE_C_ADDITIONAL_FLAGS}")

# Conditionally dump AST if requested
if (CLANG_DUMP_AST)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xclang -ast-dump -Xclang -ast-dump-filter=${EXECUTABLE_NAME} -o ${CMAKE_BINARY_DIR}/${EXECUTABLE_NAME}.ast")
    message(STATUS "AST dump will be generated at ${CMAKE_BINARY_DIR}/${EXECUTABLE_NAME}.ast")
else()
endif()

# Conditionally set flags based on what is passed to CMake
if (CLANG_VERBOSE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -v")
endif()

# Print the compiler flags for debugging purposes
message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# Print the actual compiler flags that will be used

# Add common flags specifically to the target, excluding e.g. raylib warnings
add_compile_options(
    -Wno-everything
    $<$<COMPILE_LANGUAGE:C>:-Wno-reserved-id-macro>
    $<$<COMPILE_LANGUAGE:C>:-Wno-documentation>
    $<$<COMPILE_LANGUAGE:C>:-Wno-documentation-unknown-command>
)

project(${EXECUTABLE_NAME} LANGUAGES C)

if (INCLUDE_TESTS)
    message(STATUS "Including tests in the build.")
    enable_testing()
    add_subdirectory(tests/scratch)
    add_subdirectory(tests/unit)
    add_subdirectory(tests/integration)
    add_subdirectory(tests/benchmark)
    add_subdirectory(tests/e2e)
endif()

# Add source to this project's executable.
file(GLOB_RECURSE CA_RENDERER_SRC CONFIGURE_DEPENDS "src/*.c")
add_executable(${EXECUTABLE_NAME} "${CMAKE_SOURCE_DIR}/resources" ${CA_RENDERER_SRC})

# Set cache dirs
set(RAYLIB_CACHE_DIR "${CMAKE_SOURCE_DIR}/external/raylib-${RAYLIB_VERSION}")
set(TRACY_CACHE_DIR "${CMAKE_SOURCE_DIR}/external/tracy-${TRACY_VERSION}")

# Raylib

# First check if raylib is already built in our cache directory
if(EXISTS "${RAYLIB_CACHE_DIR}/build")
    message(STATUS "Using pre-built raylib from ${RAYLIB_CACHE_DIR}/build")
    set(raylib_DIR "${RAYLIB_CACHE_DIR}/build/raylib")
endif()

# Then try to find raylib
find_package(raylib ${RAYLIB_VERSION} QUIET)

# Only download and build if not found
if (NOT raylib_FOUND)
    include(FetchContent)
    
    # Check if we have source cached
    if(EXISTS "${RAYLIB_CACHE_DIR}")
        message(STATUS "Using cached raylib source from ${RAYLIB_CACHE_DIR}")
        FetchContent_Declare(
            raylib
            SOURCE_DIR "${RAYLIB_CACHE_DIR}"
        )
    else()
        message(STATUS "Downloading raylib ${RAYLIB_VERSION}")
        FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
            SOURCE_DIR "${RAYLIB_CACHE_DIR}"
        )
    endif()

    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        set(GRAPHICS ${GRAPHICS_LEVEL})
        FetchContent_MakeAvailable(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    endif()
endif()


# Platform-specific libraries to link against
set(PLATFORM_LIBS "")

enable_language(CXX)
# Set C++ flags to match C flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

include(FetchContent)
FetchContent_Declare(
    tracy
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    GIT_REPOSITORY
    https://github.com/wolfpld/tracy.git
    GIT_TAG
    v${TRACY_VERSION}
    GIT_SHALLOW
    TRUE
    GIT_PROGRESS
    TRUE
    SOURCE_DIR "${TRACY_CACHE_DIR}"
)

FetchContent_MakeAvailable(tracy)

set_target_properties(TracyClient PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# link against static C++ standard library
target_link_options(TracyClient PRIVATE -static-libgcc -static-libstdc++ -static-libwinpthread)

if (TRACY_ENABLE)
    message(STATUS "Adding Tracy profiler support to compile definitions.")
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE TRACY_ENABLE)
else()
    message(STATUS "Tracy profiler support disabled.")
endif()
target_link_libraries(${EXECUTABLE_NAME} PRIVATE TracyClient)

if(MSVC OR MINGW OR MINGW32 OR MINGW64)
    # For MSVC or MinGW, link against Windows libraries
    list(APPEND PLATFORM_LIBS raylib kernel32 user32 gdi32 winmm shell32 )
else()
    # Other compilers, possibly Unix-like
    list(APPEND PLATFORM_LIBS raylib m c)
endif()

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    ${PLATFORM_LIBS}
)

# Include additional directories
target_include_directories(
    ${EXECUTABLE_NAME}
    PRIVATE
    src "external/raylib-${RAYLIB_VERSION}/src"
    $<$<CONFIG:DEBUG>:${TRACY_CACHE_DIR}/public>  # Add Tracy headers in Debug mode
)

# Copy resources directory to the output directory
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/resources)