cmake_minimum_required(VERSION 3.10)


project(fac VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include_directories("include/")

file(GLOB_RECURSE FAC_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM FAC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(fac_static STATIC ${FAC_SOURCES})

add_library(fac_shared SHARED ${FAC_SOURCES})

set_target_properties(fac_static PROPERTIES POSITION_INDEPENDENT_CODE ON)

set_target_properties(fac_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(fac_static PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(fac_shared PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fac_static PUBLIC IS_FAC_BUILT=1)

target_compile_definitions(fac_shared PUBLIC IS_FAC_BUILT=1)

install(TARGETS fac_shared EXPORT facTargets DESTINATION lib)
install(TARGETS fac_static EXPORT facTargets DESTINATION lib)
install(DIRECTORY "include/" DESTINATION include)

install(EXPORT facTargets
    FILE facTargets.cmake
    NAMESPACE fac::
    DESTINATION lib/cmake/fac
)

add_executable(factools "src/main.cpp")

target_link_libraries(factools PRIVATE fac_shared)