#!/bin/bash

function print_usage {
    echo_red "Usage: build COMMAND."
    echo_yellow "where COMMAND is one of:"
    echo_yellow "  -d           \t run release unit tests(default: release unit test cases)."
    echo_yellow "  -h           \t show help."
}

BIN_DIR=$(cd `dirname $0`; pwd)
CMAKELISTS_ROOT_DIR=${BIN_DIR}/..
UT_DEBUG_DIR="build/ut/debug"
UT_DEBUG_ROOT_DIR_PATH=${BIN_DIR}/../${UT_DEBUG_DIR}

UT_RELEASE_DIR="build/ut/release"
REAL_UT_ROOT_DIR_PATH=${BIN_DIR}/../${UT_RELEASE_DIR}

# load color utils
. ${BIN_DIR}/utils/color-utils.sh

if [ "$1" != "" -a "$1" != "-d" ]; then
    print_usage
    exit 1
fi

if [ 1 -eq $# ]; then
    echo_yellow "start to run all debug unit test..."
    REAL_UT_ROOT_DIR_PATH=${UT_DEBUG_ROOT_DIR_PATH}
elif [ "$1" != "-d" ]; then
    echo_yellow "start to run all release unit test..."
fi

echo "REAL_UT_ROOT_DIR_PATH = ${REAL_UT_ROOT_DIR_PATH}"

for ut in `ls ${REAL_UT_ROOT_DIR_PATH}`
do
    ut_exe=`ls ${REAL_UT_ROOT_DIR_PATH}/$ut`
    echo_yellow "running case ${ut_exe}"
    ${ut_exe}
done

echo_yellow "run all uts over."
