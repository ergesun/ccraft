#!/bin/bash

BIN_DIR=$(cd `dirname $0`; pwd)
PROJECT_ROOT_DIR=${BIN_DIR}/..
THIRDPARTY_ROOT=${PROJECT_ROOT_DIR}/third_party
THIRDPARTY_DOWN_ROOT=${THIRDPARTY_ROOT}/downloads

# check env
INS_CMD="wrongwrongwrong"

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

#####################################################################################################################
# gtest
cd ${THIRDPARTY_DOWN_ROOT}
git clone https://github.com/google/googletest.git

######################################################################################################################
# libevent
cd ${THIRDPARTY_DOWN_ROOT}
COMPONENT_DOWN_ROOT=${THIRDPARTY_DOWN_ROOT}/libevent-2.0.21-stable
wget https://github.com/downloads/libevent/libevent/libevent-2.0.21-stable.tar.gz

######################################################################################################################
# boost
cd ${THIRDPARTY_DOWN_ROOT}

wget http://sourceforge.net/projects/boost/files/boost/1.60.0/boost_1_60_0.tar.gz

######################################################################################################################
# thrift
if [ "$INS_CMD" == "wrongwrongwrong" ];then
    echo "cannot prepare thrift install env"
    exit 1
fi

cd ${THIRDPARTY_DOWN_ROOT}

if [ "apt-get" == "$INS_CMD" ];then
    sudo $INS_CMD install automake bison flex g++ git libevent-dev libssl-dev libtool make pkg-config
    wget http://ftp.debian.org/debian/pool/main/a/automake-1.15/automake_1.15-6_all.deb
    sudo dpkg -i automake_1.15-6_all.deb
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

    wget http://ftp.gnu.org/gnu/bison/bison-2.5.1.tar.gz
    tar xvf bison-2.5.1.tar.gz
    cd bison-2.5.1
    ./configure --prefix=/usr
    make
    sudo make install
    cd ..

    sudo yum -y install libevent-devel zlib-devel openssl-devel
fi

cd ${THIRDPARTY_DOWN_ROOT}
wget http://apache.01link.hk/thrift/0.10.0/thrift-0.10.0.tar.gz

######################################################################################################################
# thrift idl generator
cd ${THIRDPARTY_DOWN_ROOT}
git clone https://git-wip-us.apache.org/repos/asf/thrift.git
