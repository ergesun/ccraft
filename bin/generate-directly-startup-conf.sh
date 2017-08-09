# 解析conf文件夹下的servers文件并生成对应的uploader进程配置文件

#!/bin/bash

BIN_DIR=$(cd `dirname $0`; pwd)
. ${BIN_DIR}/../conf/acc-common-def.sh

# load color utils
. ${BIN_DIR}/utils/color-utils.sh

if [ 2 -ne $# ];then
    echo_red "ERROR: you must input generated conf root dir and startup conf root dir."
    exit 1
fi

CONF_ROOT_DIR=$1

if [ ! -d "${CONF_ROOT_DIR}" ]; then
    mkdir -p ${CONF_ROOT_DIR}
fi

ACC_CONFIG_GEN_PATH=

function gen_config_file_path() {
    ACC_CONFIG_GEN_PATH="${CONF_ROOT_DIR}/acc${1}.conf"
}

declare -i idx=0
for p in `cat ${BIN_DIR}/../conf/servers|grep -v ^#`
do
    if [ $idx -eq 0 ]; then
        THRIFT_SERV_PORT=${p}
    elif [ $idx -eq 1 ]; then
        HTTP_SERV_PORT=${p}
    else
        echo_red "ERROR: extra port $p!"
        exit 1
    fi

    idx=$((idx+1))
done

BIZ_CONF_FILE_PATH=$2
gen_config_file_path ${THRIFT_SERV_PORT}
. ${BIN_DIR}/../conf/${ACC_CONFIG_NAME}
echo_green "generate config ${ACC_CONFIG_GEN_PATH}"
