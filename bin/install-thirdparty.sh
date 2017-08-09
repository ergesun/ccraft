#!/bin/bash

BIN_DIR=$(cd `dirname $0`; pwd)
PROJECT_ROOT_DIR=${BIN_DIR}/..
THIRDPARTY_ROOT=${PROJECT_ROOT_DIR}/third_party
THIRDPARTY_DOWN_ROOT=${THIRDPARTY_ROOT}/downloads


######################################################################################################################
# gflags
cd ${THIRDPARTY_DOWN_ROOT}
COMPONENT_DOWN_ROOT=${THIRDPARTY_DOWN_ROOT}/gflags-2.2.0

unzip v2.2.0
cd ${COMPONENT_DOWN_ROOT}
mkdir ${COMPONENT_DOWN_ROOT}/build; cd ${COMPONENT_DOWN_ROOT}/build
cmake -DBUILD_STATIC_LIBS=on -DBUILD_SHARED_LIBS=on -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_ROOT} ..
make -j4
make install


######################################################################################################################
# glog
cd ${THIRDPARTY_DOWN_ROOT}
COMPONENT_DOWN_ROOT=${THIRDPARTY_DOWN_ROOT}/glog-0.3.4

unzip v0.3.4
cd ${COMPONENT_DOWN_ROOT}
#export LDFLAGS=-L${THIRDPARTY_ROOT}/lib
#export CPPFLAGS=-I${THIRDPARTY_ROOT}/include
env CPPFLAGS=-I${THIRDPARTY_ROOT}/include LDFLAGS=-L${THIRDPARTY_ROOT}/lib ./configure --prefix=${THIRDPARTY_ROOT}
make -j4
make install


######################################################################################################################
# gtest
cd ${THIRDPARTY_DOWN_ROOT}
COMPONENT_DOWN_ROOT=${THIRDPARTY_DOWN_ROOT}/googletest

cd ${COMPONENT_DOWN_ROOT}
mkdir ${COMPONENT_DOWN_ROOT}/build; cd ${COMPONENT_DOWN_ROOT}/build
cmake -DGTEST_LANG_CXX11=1 -DGTEST_HAS_TR1_TUPLE=0 -DGTEST_USE_OWN_TR1_TUPLE=0 -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_ROOT} ..
make -j4
make install
