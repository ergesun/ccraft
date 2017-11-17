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

###add_subdirectory(${BACKEND_ROOT}/src/common)
##---------------------------------for clion debug ut-------------------------------------------
#add_definitions(-DGTEST_LANG_CXX11=1 -DGTEST_HAS_TR1_TUPLE=0 -DGTEST_USE_OWN_TR1_TUPLE=0)
#add_subdirectory(${SRC_ROOT}/ccsys)
#add_subdirectory(${SRC_ROOT}/fsio)
#add_subdirectory(${SRC_ROOT}/common)
#add_subdirectory(${SRC_ROOT}/net)
#add_subdirectory(${SRC_ROOT}/rpc)
#add_subdirectory(${SRC_ROOT}/codegen)
#
#set(BUILD_OUTPUT_ROOT_DIR ${PROJECT_SOURCE_DIR}/build/ut/debug)
#set(LIBRARY_OUTPUT_PATH ${BUILD_OUTPUT_ROOT_DIR}/lib)
#set(EXECUTABLE_OUTPUT_PATH ${BUILD_OUTPUT_ROOT_DIR})
#add_subdirectory(${UT_ROOT}/rpc)
##----------------------------------------------------------------------------------------------

if(${PROJECT_TYPE} MATCHES "UT")
    message(STATUS "UT_MODULE=${UT_MODULE}")
    add_definitions(-DGTEST_LANG_CXX11=1 -DGTEST_HAS_TR1_TUPLE=0 -DGTEST_USE_OWN_TR1_TUPLE=0)
    if(NOT ${UT_MODULE} STREQUAL "fsio")
        add_subdirectory(${SRC_ROOT}/ccsys)
        if(NOT ${UT_MODULE} STREQUAL "ccsys")
            add_subdirectory(${SRC_ROOT}/common)
            if(${UT_MODULE} STREQUAL "net")
                add_subdirectory(${SRC_ROOT}/net)
            elseif(${UT_MODULE} STREQUAL "rpc")
                add_subdirectory(${SRC_ROOT}/net)
                add_subdirectory(${SRC_ROOT}/rpc)
                add_subdirectory(${SRC_ROOT}/codegen)
            endif()
        endif()
    else()
        add_subdirectory(${SRC_ROOT}/fsio)
    endif()

    message(STATUS "UT_ROOT/UT_MODULE = ${UT_ROOT}/${UT_MODULE}")
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(BUILD_OUTPUT_ROOT_DIR ${PROJECT_SOURCE_DIR}/build/ut/release)
    else()
        set(BUILD_OUTPUT_ROOT_DIR ${PROJECT_SOURCE_DIR}/build/ut/debug)
    endif()
    set(LIBRARY_OUTPUT_PATH ${BUILD_OUTPUT_ROOT_DIR}/lib)
    set(EXECUTABLE_OUTPUT_PATH ${BUILD_OUTPUT_ROOT_DIR})
    add_subdirectory(${UT_ROOT}/${UT_MODULE})
else()
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(BUILD_OUTPUT_ROOT_DIR ${PROJECT_SOURCE_DIR}/build/ccraft/release)
    else()
        set(BUILD_OUTPUT_ROOT_DIR ${PROJECT_SOURCE_DIR}/build/ccraft/debug)
    endif()
    set(LIBRARY_OUTPUT_PATH ${BUILD_OUTPUT_ROOT_DIR}/lib)
    set(EXECUTABLE_OUTPUT_PATH ${BUILD_OUTPUT_ROOT_DIR})
    add_subdirectory(${SRC_ROOT})
endif()

link_directories(${BUILD_OUTPUT_ROOT_DIR}/lib)
