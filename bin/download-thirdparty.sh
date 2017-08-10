#!/bin/bash

BIN_DIR=$(cd `dirname $0`; pwd)
PROJECT_ROOT_DIR=${BIN_DIR}/..
THIRDPARTY_ROOT=${PROJECT_ROOT_DIR}/third_party
THIRDPARTY_DOWN_ROOT=${THIRDPARTY_ROOT}/downloads

# check env
INS_CMD="deadcmdcmddead"

if command -v yum >/dev/null 2>&1; then
  INS_CMD="yum"
fi

if command -v apt-get >/dev/null 2>&1; then
  INS_CMD="apt-get"
fi

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

######################################################################################################################
# gtest
cd ${THIRDPARTY_DOWN_ROOT}
git clone https://github.com/google/googletest.git


######################################################################################################################
# proto buf
## prepare for pb
if [ "apt-get" == "$INS_CMD" ];then
    sudo apt-get install autoconf automake libtool curl make g++ unzip
elif [ "yum" == "$INS_CMD" ];then
    sudo yum -y update
    sudo yum -y groupinstall "Development Tools"
    sudo yum install -y wget
    wget http://ftp.gnu.org/gnu/autoconf/autoconf-2.69.tar.gz
    tar xvf autoconf-2.69.tar.gz
    cd autoconf-2.69
    ./configure --prefix=/usr
    make
    sudo make install
    cd ..

    sudo yum -y install curl unzip
fi

cd ${THIRDPARTY_DOWN_ROOT}
wget https://codeload.github.com/google/protobuf/tar.gz/v3.3.2
