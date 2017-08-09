#!/bin/bash

BIN_DIR=$(cd `dirname $0`; pwd)

. ${BIN_DIR}/utils/color-utils.sh

ROOT_DIR=${BIN_DIR}/..
. ${ROOT_DIR}/conf/acc-common-def.sh

function print_usage {
    echo_red "Usage: pack COMMAND."
    echo_yellow "where COMMAND is one of:"
    echo_yellow "  --without-gen    \t without call generate supervisor conf sh."
    echo_yellow "  --without-build  \t without call build.sh."
    echo_yellow "  -h               \t show help."
}

if [ ! -f "${ROOT_DIR}/conf/acc-biz.json" ]; then
    echo_red "ERROR: you must write a file named acc-biz.json in ${ROOT_DIR}/conf."
    exit 1
fi

WITHOUT_GEN=
WITHOUT_BUILD=

for p in "$@"
do
    if [ "$p" = "--without-gen" ]; then
        WITHOUT_GEN="1"
    elif [ "$p" = "--without-build" ]; then
        WITHOUT_BUILD="1"
        ${BIN_DIR}/generate-supervisor-conf.sh
    elif [ "$p" == "-h" ]; then
        print_usage
        exit 0
    else
        echo_red "Not support opt \"$p\""
        print_usage
        exit 1
    fi
done

if [ "${WITHOUT_BUILD}" == "" ]; then
    echo_yellow "running build.sh -gen..."
    ${BIN_DIR}/build.sh -gen
fi

if [ "${WITHOUT_GEN}" == "" ]; then
    echo_yellow "running generate supervisor conf sh..."
    ${BIN_DIR}/generate-supervisor-conf.sh
fi

echo_yellow "packing targets into ${PACK_ROOT_DIR}..."

mkdir -p ${PACK_ROOT_DIR}
mkdir -p ${PACK_BIN_DIR}
mkdir -p ${PACK_CONF_ROOT_DIR}
mkdir -p ${PACK_ACC_CONF_DIR}
mkdir -p ${PACK_CONF_SUPERVISOR_CHILDREN_DIR}

# TODO(sunchao): 等和测试组对接完之后看看依赖的库是否都是静态连接，否则打包安装的时候需要加上动态连接的库。
cp -r ${BIN_DIR}/utils                         ${PACK_BIN_DIR}
cp ${BIN_DIR}/install-acc.sh                   ${PACK_BIN_DIR}
cp ${BIN_DIR}/${ACC_BINARY_FILE_NAME}          ${PACK_BIN_DIR}
cp ${BIN_DIR}/start-acc-by-supervisor.sh       ${PACK_BIN_DIR}
cp -r ${GEN_ACC_CONF_DIR}/*                    ${PACK_ACC_CONF_DIR}
cp ${ROOT_DIR}/conf/acc-common-def.sh          ${PACK_CONF_ROOT_DIR}
cp ${GEN_SUPERVISOR_CONF_FILE_PATH}            ${PACK_CONF_ROOT_DIR}
cp -r ${GEN_SUPERVISOR_CHILDREN_CONF_DIR}/*    ${PACK_CONF_SUPERVISOR_CHILDREN_DIR}
cp ${ROOT_DIR}/conf/acc-biz.json               ${PACK_CONF_ROOT_DIR}

tar -zcvf ${ROOT_DIR}/acc-dist.tgz ${PACK_ROOT_DIR}

if [ $? -eq 0 ]; then
    echo_green "pack acc successfully!"
else
    echo_red "pack acc failed!"
fi
