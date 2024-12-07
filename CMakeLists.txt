cmake_minimum_required(VERSION 3.10)
project(VDSProject CXX C)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC -std=c++17 ${LLVM_CXX_FLAGS} ")
include(CTest)
include(FetchContent)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Options
option(CLASSPROJECT_COVERAGE "Enable coverage reporting for class project" OFF)
option(CLASSPROJECT_VISUALIZE "Enable visualization for class project" OFF)

# Coverage flags
if (CLASSPROJECT_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif ()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in ${CMAKE_SOURCE_DIR}/gtest/googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -D "CMAKE_C_COMPILER=${CMAKE_C_COMPILER}" -D "CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}" -D "CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}" -D "CMAKE_C_FLAGS=${CMAKE_C_FLAGS}" -D "CMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}" -D "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" -D "CMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/gtest/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/gtest/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Optimazition flags
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Set -march and -mtune flags
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mtune=native")
endif()

if(CLASSPROJECT_VISUALIZE)
    # Find the Graphviz libraries
    find_library(GRAPHVIZ_LIBRARIE_GVC NAMES graphviz gvc)
    find_library(GRAPHVIZ_LIBRARIE_CGRAPH NAMES graphviz cgraph)
    # find_library(GRAPHVIZ_LIBRARIE_CDT NAMES graphviz cdt)
    # find_library(GRAPHVIZ_LIBRARIE_CORE NAMES graphviz gvplugin_core)
    # find_library(GRAPHVIZ_LIBRARIE_DOT_LAYOUT NAMES graphviz gvplugin_dot_layout)
    # find_library(GRAPHVIZ_LIBRARIE_GD NAMES graphviz gvplugin_gd)
    
    # Concatenate the libraries
    if (GRAPHVIZ_LIBRARIE_GVC AND GRAPHVIZ_LIBRARIE_CGRAPH) #AND GRAPHVIZ_LIBRARIE_CDT AND GRAPHVIZ_LIBRARIE_CORE AND GRAPHVIZ_LIBRARIE_DOT_LAYOUT AND GRAPHVIZ_LIBRARIE_GD)
        set(GRAPHVIZ_LIBRARIES ${GRAPHVIZ_LIBRARIE_GVC} ${GRAPHVIZ_LIBRARIE_CGRAPH})# ${GRAPHVIZ_LIBRARIE_CDT} ${GRAPHVIZ_LIBRARIE_CORE} ${GRAPHVIZ_LIBRARIE_DOT_LAYOUT} ${GRAPHVIZ_LIBRARIE_GD})
    else ()
        set(GRAPHVIZ_LIBRARIES "")
    endif ()
    # Find the include directory
    find_path(GRAPHVIZ_INCLUDE_DIR NAMES graphviz/gvc.h)
    # Check if found
    if (GRAPHVIZ_LIBRARIES AND GRAPHVIZ_INCLUDE_DIR)
        message(STATUS "Found Graphviz libraries: ${GRAPHVIZ_LIBRARIES}")
        message(STATUS "Graphviz include directory: ${GRAPHVIZ_INCLUDE_DIR}")
    else ()
        message(FATAL_ERROR "Graphviz library not found!")
    endif ()
endif()

# Include prebuild Boost
set(BOOST_INCLUDE_LIBRARIES system) # Add system library for boost/spirit
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
FetchContent_Declare(
  boost
  URL https://archives.boost.io/release/1.86.0/source/boost_1_86_0.tar.gz
  URL_HASH SHA256=2575e74ffc3ef1cd0babac2c1ee8bdb5782a0ee672b1912da40e5b4b591ca01f
)
FetchContent_MakeAvailable(boost)


# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_SOURCE_DIR}/gtest/googletest-src
        ${CMAKE_SOURCE_DIR}/gtest/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()

#Include dirs
include_directories(${CMAKE_SOURCE_DIR}/lib)
link_directories(${CMAKE_SOURCE_DIR}/lib)

include_directories(${CMAKE_SOURCE_DIR}/src)
link_directories(${CMAKE_SOURCE_DIR}/src)

include_directories(${CMAKE_SOURCE_DIR}/src/test/)
link_directories(${CMAKE_SOURCE_DIR}/src/test/)

add_subdirectory(src/bench)
include_directories(${CMAKE_SOURCE_DIR}/src/bench/)
link_directories(${CMAKE_SOURCE_DIR}/src/bench/)

add_subdirectory(src/verify)
include_directories(${CMAKE_SOURCE_DIR}/src/verify/)
link_directories(${CMAKE_SOURCE_DIR}/src/verify/)

add_subdirectory(src)
add_executable(VDSProject src/main.cpp)
target_link_libraries(VDSProject Manager)

# Coverage
if (CLASSPROJECT_COVERAGE)
# lcov and genhtml are required for coverage
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    add_custom_target(coverage #ALL
        COMMAND lcov --capture --directory . --output-file lcov.info
        COMMAND genhtml lcov.info --output-directory coverage_report
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating code coverage report"
    )
endif ()
