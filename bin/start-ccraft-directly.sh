#!/bin/bash
BIN_DIR=$(cd `dirname $0`; pwd)
. ${BIN_DIR}/../conf/acc-common-def.sh
ACC_PATH="${BIN_DIR}/${ACC_BINARY_FILE_NAME}"

echo "# 定义服务实例的端口
# thrift server
9905

# http server
8088
" > ${BIN_DIR}/../conf/servers

. ${BIN_DIR}/generate-directly-startup-conf.sh /tmp/acc ${BIN_DIR}/../conf/eg--acc-biz.json

echo "load config ${ACC_CONFIG_GEN_PATH} and start access_control_center..."
${ACC_PATH} --flagfile=${ACC_CONFIG_GEN_PATH}
