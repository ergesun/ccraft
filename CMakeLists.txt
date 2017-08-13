cmake_minimum_required(VERSION 3.5)
project(ccraft)

# debug/release编译控制
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3")

# 自定义宏
option(DEFINE_MACRO "Option for [debug logs] macro" OFF)
if(DEFINE_MACRO)
    ADD_DEFINITIONS(-DDEBUG_LOG)
endif(DEFINE_MACRO)

# 使用c++11标准
set(CMAKE_CXX_STANDARD 11)

# 添加gcc flags
## -Wno-unused-result：忽略未接返回值的warning
## -Wno-unused-function：忽略定义的函数未使用的警告
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-function -Wno-unused-variable -Wno-unused-result")

# 显示make详细信息
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(SRC_ROOT ${PROJECT_SOURCE_DIR}/src)
set(UT_ROOT ${SRC_ROOT}/unit-test)
#set(TL_ROOT ${PROJECT_SOURCE_DIR}/tools)
set(EXTERN_COMPILE_THIRDPARTY_ROOT ${PROJECT_SOURCE_DIR}/third_party)

# 第三方库
include_directories(${EXTERN_COMPILE_THIRDPARTY_ROOT}/include)
message(STATUS "EXTERN_COMPILE_THIRDPARTY_ROOT = ${EXTERN_COMPILE_THIRDPARTY_ROOT}")
link_directories(${EXTERN_COMPILE_THIRDPARTY_ROOT}/lib)

#add_subdirectory(${BACKEND_ROOT}/src/common)
#add_subdirectory(${UT_ROOT}/common)
if(${PROJECT_TYPE} MATCHES "UT")
    add_definitions(-DGTEST_LANG_CXX11=1 -DGTEST_HAS_TR1_TUPLE=0 -DGTEST_USE_OWN_TR1_TUPLE=0)
    if(${UT_MODULE} MATCHES "common")
        add_subdirectory(${SRC_ROOT}/common)
    endif()
    message(STATUS "UT_ROOT/UT_MODULE = ${UT_ROOT}/${UT_MODULE}")
    if(${CMAKE_BUILD_TYPE} MATCHES "Release")
        set(BUILD_OUTPUT_ROOT_DIR ${PROJECT_SOURCE_DIR}/build/ut/release)
    else()
        set(BUILD_OUTPUT_ROOT_DIR ${PROJECT_SOURCE_DIR}/build/ut/debug)
    endif()
    set(LIBRARY_OUTPUT_PATH ${BUILD_OUTPUT_ROOT_DIR}/lib)
    set(EXECUTABLE_OUTPUT_PATH ${BUILD_OUTPUT_ROOT_DIR})
    add_subdirectory(${UT_ROOT}/${UT_MODULE})
else()
    if(${CMAKE_BUILD_TYPE} MATCHES "Release")
        set(BUILD_OUTPUT_ROOT_DIR ${PROJECT_SOURCE_DIR}/build/ccraft/release)
    else()
        set(BUILD_OUTPUT_ROOT_DIR ${PROJECT_SOURCE_DIR}/build/ccraft/debug)
    endif()
    set(LIBRARY_OUTPUT_PATH ${BUILD_OUTPUT_ROOT_DIR}/lib)
    set(EXECUTABLE_OUTPUT_PATH ${BUILD_OUTPUT_ROOT_DIR})
    add_subdirectory(${SRC_ROOT})
endif()

link_directories(${BUILD_OUTPUT_ROOT_DIR}/lib)
