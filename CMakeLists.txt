cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FAC_STATIC "Build the library as static (default OFF)" OFF)

set(LIBRARY_TYPE STATIC)
if(NOT FAC_STATIC)
    set(LIBRARY_TYPE SHARED)
endif()

project(fac)

project(factools)

include_directories("include/")

file(GLOB_RECURSE FAC_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM FAC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")



add_library(fac ${LIBRARY_TYPE} ${FAC_SOURCES})

set_target_properties(fac PROPERTIES POSITION_INDEPENDENT_CODE ON)


target_compile_definitions(fac PUBLIC IS_FAC_BUILT=1)

install(TARGETS fac DESTINATION lib)
install(DIRECTORY "include/" DESTINATION include)


add_executable(factools "src/main.cpp")

target_link_libraries(factools fac)