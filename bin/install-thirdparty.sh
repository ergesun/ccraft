#!/bin/bash

# TODO(sunchao): 本应该安装到项目目录中，但是因为thrift找不到boost，暂时放到系统目录下。

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
cmake -DBUILD_STATIC_LIBS=on -DBUILD_SHARED_LIBS=on ..
make -j4
sudo make install


######################################################################################################################
# glog
cd ${THIRDPARTY_DOWN_ROOT}
COMPONENT_DOWN_ROOT=${THIRDPARTY_DOWN_ROOT}/glog-0.3.4

unzip v0.3.4
cd ${COMPONENT_DOWN_ROOT}
./configure
make -j4
sudo make install


######################################################################################################################
# gtest
cd ${THIRDPARTY_DOWN_ROOT}
COMPONENT_DOWN_ROOT=${THIRDPARTY_DOWN_ROOT}/googletest

cd ${COMPONENT_DOWN_ROOT}
mkdir ${COMPONENT_DOWN_ROOT}/build; cd ${COMPONENT_DOWN_ROOT}/build
cmake -DGTEST_LANG_CXX11=1 -DGTEST_HAS_TR1_TUPLE=0 -DGTEST_USE_OWN_TR1_TUPLE=0 ..
make -j4
sudo make install


######################################################################################################################
# libevent
cd ${THIRDPARTY_DOWN_ROOT}
COMPONENT_DOWN_ROOT=${THIRDPARTY_DOWN_ROOT}/libevent-2.0.21-stable

tar -zxvf libevent-2.0.21-stable.tar.gz
cd ${COMPONENT_DOWN_ROOT}
./configure
make -j4
sudo make install


######################################################################################################################
# boost
cd ${THIRDPARTY_DOWN_ROOT}
tar xvf boost_1_60_0.tar.gz
cd boost_1_60_0
# 貌似thrift有bug，boost指定安装的话，即便thrift指定boost位置还是有问题。
#./bootstrap.sh --prefix=${THIRDPARTY_ROOT}
./bootstrap.sh
sudo ./b2 install

######################################################################################################################
# thrift idl generator
cd ${THIRDPARTY_DOWN_ROOT}/thrift
./bootstrap.sh
./configure --with-lua=no
make -j4
sudo make install


######################################################################################################################
# thrift
cd ${THIRDPARTY_DOWN_ROOT}
tar -zxvf thrift-0.10.0.tar.gz
cd thrift-0.10.0
#./bootstrap.sh
# 参考boost处的说明。
#./configure --with-boost=${THIRDPARTY_ROOT} --prefix=${THIRDPARTY_ROOT}
./configure
make -j4
sudo make install
