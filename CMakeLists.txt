cmake_minimum_required(VERSION 3.6)
project(ccraft)

# debug/release编译控制
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3")

# 自定义宏
option(DEFINE_MACRO "Option for [debug logs] macro" OFF)
option(UT_MACRO "Option for [unit test] macro" OFF)
if(DEFINE_MACRO)
    ADD_DEFINITIONS(-DDEBUG_LOG)
endif(DEFINE_MACRO)

# 使用c++11标准
set(CMAKE_CXX_STANDARD 11)

# 添加gcc flags
## -Wno-unused-result：忽略未接返回值的warning
## -Wno-unused-function：忽略定义的函数未使用的警告
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-function -Wno-unused-variable")

# 显示make详细信息
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(SRC_ROOT ${PROJECT_SOURCE_DIR}/src)
set(UT_ROOT ${SRC_ROOT}/unit-test)
#set(TL_ROOT ${PROJECT_SOURCE_DIR}/tools)
set(EXTERN_COMPILE_THIRDPARTY_ROOT ${PROJECT_SOURCE_DIR}/third_party)

# 第三方库
include_directories(${EXTERN_COMPILE_THIRDPARTY_ROOT}/include)
link_directories(${EXTERN_COMPILE_THIRDPARTY_ROOT}/lib)

if (${CMAKE_BUILD_TYPE} MATCHES "Release")
    link_directories(${PROJECT_SOURCE_DIR}/lib)
else()
    message(STATUS "CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")
    set(LINK_DIRS
            ${SRC_ROOT}/common
            ${SRC_ROOT}/net
            )
    link_directories(${LINK_DIRS})
endif()

#add_subdirectory(${BACKEND_ROOT}/src/common)
#add_subdirectory(${UT_ROOT}/common)

if(${PROJECT_TYPE} MATCHES "UT")
    add_definitions(-DGTEST_LANG_CXX11=1 -DGTEST_HAS_TR1_TUPLE=0 -DGTEST_USE_OWN_TR1_TUPLE=0)
    #if (${UT_MODULE} MATCHES "rpc")
    #    add_subdirectory(${BACKEND_ROOT}/src/gen-cpp)
    #    add_subdirectory(${BACKEND_ROOT}/src/rpc)
    #elseif(${UT_MODULE} MATCHES "tb")
    #    set(THIRD_PARTY_ROOT ${BACKEND_ROOT}/src/third-party)
    #    add_subdirectory(${BACKEND_ROOT}/src/gen-cpp)
    #    add_subdirectory(${BACKEND_ROOT}/src/third-party)
    #    add_subdirectory(${BACKEND_ROOT}/src/common)
    #    add_subdirectory(${BACKEND_ROOT}/src/acc)
    #elseif(${UT_MODULE} MATCHES "common")
    #    add_subdirectory(${BACKEND_ROOT}/src/common)
    #elseif(${UT_MODULE} MATCHES "biz-conf-parser")
    #    set(THIRD_PARTY_ROOT ${BACKEND_ROOT}/src/third-party)
    #    add_subdirectory(${BACKEND_ROOT}/src/gen-cpp)
    #    add_subdirectory(${BACKEND_ROOT}/src/third-party)
    #    add_subdirectory(${BACKEND_ROOT}/src/common)
    #    add_subdirectory(${BACKEND_ROOT}/src/acc)
    #endif()
    add_subdirectory(${UT_ROOT}/${UT_MODULE})
elseif(${PROJECT_TYPE} MATCHES "TOOLS")
    #if (${TL_MODULE} MATCHES "qps")
    #    add_subdirectory(${BACKEND_ROOT}/src/gen-cpp)
    #endif()
    add_subdirectory(${TL_ROOT}/${TL_MODULE})
else()
    add_subdirectory(${SRC_ROOT})
endif()
