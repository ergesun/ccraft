#!/bin/bash

#TODO(sunchao): To improve this script codes. By the way, my bash script coding ability is poor.

BIN_DIR=$(cd `dirname $0`; pwd)
CMAKELISTS_ROOT_DIR=${BIN_DIR}/..
BUILD_OBJS_FOLDER="cmake-objs"
BUILD_OBJS_DIR=${BIN_DIR}/../${BUILD_OBJS_FOLDER}

# load color utils
. ${BIN_DIR}/utils/color-utils.sh

function print_usage {
    echo_red "Usage: build COMMAND."
    echo_yellow "where COMMAND is one of:"
    echo_yellow "  -gen         \t generate protobuf IDLs before build(default not)."
    echo_yellow "  -m           \t make args. eg: -m -j8"
    echo_yellow "  -d           \t build with debug info."
    echo_yellow "  -dl          \t build with debug log."
    echo_yellow "  -ut          \t build unit test. It should pass a unit_test module param. eg: [./build.sh -ut rpc]. Support [all] param present all unit tests。"
    echo_yellow "  --list-ut    \t list all unit test."
    echo_yellow "  -tl          \t build tools. It should pass a tool name param. eg: [./build.sh -tl qps]"
    echo_yellow "  --list-tools \t list all tools."
    echo_yellow "  -h           \t show help."
}

if [ 1 -eq $# ]; then
    if [ "--list-ut" = "$1" ]; then
        echo_green "[Unit tests]:"
        ls ${CMAKELISTS_ROOT_DIR}/src/unit-test | grep -v "readme"
        exit 0
    elif [ "--list-tools" = "$1" ]; then
        echo_green "[Tools]:"
        ls ${CMAKELISTS_ROOT_DIR}/src/tools
        exit 0
    fi
fi

mkdir -p ${BUILD_OBJS_DIR}
cd ${BUILD_OBJS_DIR}

BUILD_FLAG="-DCMAKE_BUILD_TYPE=Release"
BUILD_PROJECT_TYPE="Project type: ccraft"
BUILD_TYPE_INFO="BuildType: Release with \"-O3\""
BUILD_LOG_INFO="DebugLog: off"
GEN_IDLS=

echo_yellow "[Phase 0]: parse params..."
echo_yellow "------------------------------------------------------------------------------------"
echo " "

MAKE_ARGS_STAT=
MAKE_ARGS=

UT_STAT=
UT_MODULE=

TL_STAT=
TL_MODULE=

for p in "$@"
do
    if [ "$p" = "-d" ]; then
        BUILD_FLAG="${BUILD_FLAG} -DCMAKE_BUILD_TYPE=Debug"
        BUILD_TYPE_INFO="BuildType: Debug with \"-O0 -g\""
    elif [ "$p" = "-dl" ]; then
        BUILD_FLAG="${BUILD_FLAG} -DDEFINE_MACRO=ON"
        BUILD_LOG_INFO="DebugLog: on"
    elif [ "$p" == "-gen" ]; then
        GEN_IDLS="true"
    elif [ "$p" == "-ut" ]; then
        BUILD_FLAG="${BUILD_FLAG} -DPROJECT_TYPE=UT"
        BUILD_PROJECT_TYPE="ProjectType: ccraft unit-test"
        UT_STAT="1"
    elif [ "$p" == "-tl" ]; then
        BUILD_FLAG="${BUILD_FLAG} -DPROJECT_TYPE=TOOLS"
        BUILD_PROJECT_TYPE="ProjectType: tools"
        TL_STAT="1"
    elif [ "$p" == "-m" ]; then
        MAKE_ARGS_STAT="1"
    elif [ "$p" == "-h" ]; then
        print_usage
        exit 0
    else
        if [ "$UT_STAT" != "" ]; then
            UT_MODULE=$p
            BUILD_PROJECT_TYPE="${BUILD_PROJECT_TYPE} - ${UT_MODULE}"
        elif [ "$TL_STAT" != "" ]; then
            TL_MODULE=$p
            BUILD_PROJECT_TYPE="${BUILD_PROJECT_TYPE} - ${TL_MODULE}"
        elif [ "$MAKE_ARGS_STAT" != "" ]; then
            MAKE_ARGS=$p
        else
            echo_red "Not support opt \"$p\""
            print_usage
            exit 1
        fi
    fi
done

# check and configure ut module
if [ "${UT_STAT}" != "" ]; then
    if [ "${UT_MODULE}" = "" ]; then
        echo_red "ERROR: you must input a unit_test module name if you want to do a unit_test!"
        exit 1
    fi

    if [ "${UT_MODULE}" != "all" ]; then
        FOUND_UT=

        #find . -maxdepth 1 -type d|xargs basename -a| grep -v -E '^$'| grep -v -E '^\.'
        for f in `tree ${CMAKELISTS_ROOT_DIR}/src/unit-test -L 1 -d | awk '{print $2}'|grep -v -E "^$"|grep -v "directories"|grep -v "directory"`
        do
            if [ "$f" = "${UT_MODULE}" ]; then
                FOUND_UT="1"
                break
            fi
        done

        if [ "${FOUND_UT}" = "" ]; then
            echo_red "ERROR: your input unit_test module isn't existent!"
            exit 1
        fi

        BUILD_FLAG="${BUILD_FLAG} -DUT_MODULE=${UT_MODULE}"
    fi
fi

# check and configure tool
if [ "${TL_STAT}" != "" ]; then
    if [ "${TL_MODULE}" = "" ]; then
        echo_red "ERROR: you must input a tool name!"
        exit 1
    fi

    FOUND_TL=

    for f in `ls ${CMAKELISTS_ROOT_DIR}/tools`
    do
        if [ "$f" = "${TL_MODULE}" ]; then
            FOUND_TL="1"
            break
        fi
    done

    if [ "${FOUND_TL}" = "" ]; then
        echo_red "ERROR: your input tool isn't existent!"
        exit 1
    fi

    BUILD_FLAG="${BUILD_FLAG} -DTL_MODULE=${TL_MODULE}"
fi

if [ "$GEN_IDLS" != "" ]; then
   echo_yellow "[Phase 1]: generating protobuf IDLs..."
   echo_yellow "------------------------------------------------------------------------------------"
   echo " "
   ${BIN_DIR}/generate-protobuf-idls.sh -l cpp -o ${CMAKELISTS_ROOT_DIR}/be/src
fi

if [ "${UT_MODULE}" = "all" ]; then
    if [ "$GEN_IDLS" = "" ]; then
        echo_yellow "[Phase 1]: start building all unit test targets..."
    else
        echo_yellow "[Phase 2]: start building all unit test targets..."
    fi

    for f in `tree ${CMAKELISTS_ROOT_DIR}/src/unit-test -L 1 -d | awk '{print $2}'|grep -v -E "^$"|grep -v "directories"|grep -v "directory"`
    do
        CUR_BUILD_FLAG="${BUILD_FLAG} -DUT_MODULE=$f"
        echo_yellow " >>> building [${BUILD_PROJECT_TYPE}:$f, ${BUILD_TYPE_INFO}, ${BUILD_LOG_INFO}] target..."
        cmake ${CUR_BUILD_FLAG} ${CMAKELISTS_ROOT_DIR}
        make ${MAKE_ARGS}
        ERRRC=$?
        if [ "$ERRRC" != 0 ]; then
            echo_red "[Build failed!]"
            exit
        fi
    done
else
    if [ "$GEN_IDLS" = "" ]; then
        echo_yellow "[Phase 1]: building [${BUILD_PROJECT_TYPE}, ${BUILD_TYPE_INFO}, ${BUILD_LOG_INFO}] target..."
    else
        echo_yellow "[Phase 2]: building [${BUILD_PROJECT_TYPE}, ${BUILD_TYPE_INFO}, ${BUILD_LOG_INFO}] target..."
    fi

    echo_yellow "------------------------------------------------------------------------------------"
    echo " "

    cmake ${BUILD_FLAG} ${CMAKELISTS_ROOT_DIR}
    make ${MAKE_ARGS}
    ERRRC=$?
fi

echo_yellow "------------------------------------------------------------------------------------"
echo " "

if [ "$ERRRC" != 0 ]; then
    echo_red "[Build failed!]"
else
    echo_green "[Build successfully!]"
fi
