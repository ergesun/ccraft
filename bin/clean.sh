#!/bin/bash

BIN_DIR=$(cd `dirname $0`; pwd)
ROOT_DIR=${BIN_DIR}/..

. ${ROOT_DIR}/conf/ccraft-common-def.sh

BUILD_OBJS_FOLDER="cmake-objs"
BUILD_OBJS_DIR=${BIN_DIR}/../${BUILD_OBJS_FOLDER}

# load color utils
. ${BIN_DIR}/utils/color-utils.sh

function print_usage {
    echo_red "Usage: clean COMMAND."
    echo_yellow "where COMMAND is one of:"
    echo_yellow "  -conf \t clean generated supervisor conf."
    echo_yellow "  -pack \t clean packed dist."
    echo_yellow "  -all  \t clean all generated targets(reset to just git clone state)."
    echo_yellow "  -h    \t show help."
}

for p in "$@"
do
    if [ "$p" = "-conf" ]; then
        echo "cleaning generated conf..."
        rm -rf ${GEN_CONF_ROOT_DIR}
    elif [ "$p" = "-pack" ]; then
        echo "cleaning packed..."
        rm -rf ${PACK_ROOT_DIR}
        rm -rf ${ROOT_DIR}/ccraft-dist.tgz
    elif [ "$p" = "-all" ]; then
        echo "cleaning all..."
        rm -rf ${GEN_CONF_ROOT_DIR}
        rm -rf ${PACK_ROOT_DIR}
        rm -rf ${ROOT_DIR}/ccraft-dist.tgz
    elif [ "$p" == "-h" ]; then
        print_usage
        exit 0
    else
        echo_red "Not support opt \"$p\""
        print_usage
        exit 1
    fi
done

echo "cleaning cmake objs..."
rm -rf ${BUILD_OBJS_DIR}

echo "cleaning bin binary..."
rm -rf ${BIN_DIR}/ccraftd

echo "cleaning libs..."
rm -rf ${BIN_DIR}/../lib

echo "clean over."
