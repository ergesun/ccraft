#!/bin/bash
BIN_DIR=$(cd `dirname $0`; pwd)
ROOT_DIR=${BIN_DIR}/..

. ${ROOT_DIR}/conf/acc-common-def.sh

USER=`whoami`

if [ "${USER}" != "root" ]; then
    echo "ERROR: you should be root!"
    exit 1
fi

service supervisor stop
nohup supervisord -c ${INST_SUPERVISOR_CONF_FILE_PATH} &

if [ $? -eq 0 ]; then
    echo "start acc by supervisor successfully!"
else
    echo "start acc by supervisor failed!"
fi
