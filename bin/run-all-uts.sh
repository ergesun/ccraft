#!/bin/bash

BIN_DIR=$(cd `dirname $0`; pwd)
CMAKELISTS_ROOT_DIR=${BIN_DIR}/..
UT_DIR="cmake-objs/unit-test"
UT_ROOT_DIR_PATH=${BIN_DIR}/../${UT_DIR}

# load color utils
. ${BIN_DIR}/utils/color-utils.sh

echo_yellow "start to run all unit test..."

for ut in `ls ${UT_ROOT_DIR_PATH}`
do
    ut_exe=`ls ${UT_ROOT_DIR_PATH}/$ut | grep -v "Make" | grep -v "make"`
    echo_yellow "running case ${ut_exe}"
    ${UT_ROOT_DIR_PATH}/$ut/$ut_exe
done

echo_yellow "run all uts over."
