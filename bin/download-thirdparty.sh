#!/bin/bash

BIN_DIR=$(cd `dirname $0`; pwd)
PROJECT_ROOT_DIR=${BIN_DIR}/..
THIRDPARTY_ROOT=${PROJECT_ROOT_DIR}/third_party
THIRDPARTY_DOWN_ROOT=${THIRDPARTY_ROOT}/downloads

# clean old
rm -rf ${THIRDPARTY_ROOT}
mkdir -p ${THIRDPARTY_DOWN_ROOT}

######################################################################################################################
# gflags
cd ${THIRDPARTY_DOWN_ROOT}
wget https://codeload.github.com/gflags/gflags/zip/v2.2.0

######################################################################################################################
# glog
cd ${THIRDPARTY_DOWN_ROOT}
wget https://codeload.github.com/google/glog/zip/v0.3.4

#####################################################################################################################
# gtest
cd ${THIRDPARTY_DOWN_ROOT}
git clone https://github.com/google/googletest.git

