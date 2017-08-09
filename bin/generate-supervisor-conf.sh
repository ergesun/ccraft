#!/bin/bash

BIN_DIR=$(cd `dirname $0`; pwd)

. ${BIN_DIR}/utils/color-utils.sh
. ${BIN_DIR}/../conf/acc-common-def.sh

${BIN_DIR}/generate-directly-startup-conf.sh ${GEN_ACC_CONF_DIR} ${INST_CONF_DIR}/acc-biz.json

mkdir -p ${GEN_CONF_DIR}
mkdir -p ${GEN_SUPERVISOR_CHILDREN_CONF_DIR}

###################################for supervisord self###############################################
echo_yellow "generating supervisor conf..."
echo "
; supervisor config file

[unix_http_server]
file=/var/run/supervisor.sock   ; (the path to the socket file)
chmod=0700                      ; sockef file mode (default 0700)

;[inet_http_server]         ; HTTP 服务器，提供 web 管理界面
;port=127.0.0.1:9001        ; Web 管理后台运行的 IP 和端口，如果开放到公网，需要注意安全性
;username=user              ; 登录管理后台的用户名
;password=123               ; 登录管理后台的密码

[supervisord]
logfile=/var/log/supervisor/supervisord.log ; (main log file;default $CWD/supervisord.log)
pidfile=/var/run/supervisord.pid            ; (supervisord pidfile;default supervisord.pid)
childlogdir=/var/log/supervisor             ; ('AUTO' child log dir, default $TEMP)

; the below section must remain in the config file for RPC
; (supervisorctl/web interface) to work, additional interfaces may be
; added by defining them in separate rpcinterface: sections
[rpcinterface:supervisor]
supervisor.rpcinterface_factory = supervisor.rpcinterface:make_main_rpcinterface

[supervisorctl]
serverurl=unix:///var/run/supervisor.sock ; use a unix:// URL  for a unix socket

; The [include] section can just contain the "files" setting.  This
; setting can list multiple files (separated by whitespace or
; newlines).  It can also contain wildcards.  The filenames are
; interpreted as relative to this file.  Included files *cannot*
; include files themselves.
[include]
files = ${INST_SUPERVISOR_CHILDREN_CONF_DIR}/*.conf
" > ${GEN_SUPERVISOR_CONF_FILE_PATH}

if [ $? -eq 0 ]; then
    echo_green "generate supervisor conf successfully!"
else
    echo_red "generate supervisor conf failed!"
    exit 1
fi

########################################for supervisor children#########################################

echo_yellow "generating supervisor children conf..."

CHILDREN_CONFIG_GEN_PATH=

function gen_config_file_path() {
    CHILDREN_CONFIG_GEN_PATH="${GEN_SUPERVISOR_CHILDREN_CONF_DIR}/acc${1}.conf"
}

for p in `cat ${BIN_DIR}/../conf/servers|grep -v ^#`
do
  THRIFT_SERV_PORT=${p}
  gen_config_file_path ${THRIFT_SERV_PORT}
  echo "[program:access-control-service${THRIFT_SERV_PORT}]
command=${INST_ROOT_DIR}/bin/${ACC_BINARY_FILE_NAME} --flagfile=${INST_ACC_CONF_DIR}/acc${THRIFT_SERV_PORT}.conf ; 启动命令
stdout_logfile=/var/log/supervisor/acc${THRIFT_SERV_PORT}_out.log ; stdout 日志输出位置
stderr_logfile=/var/log/supervisor/acc${THRIFT_SERV_PORT}_err.log ; stderr 日志输出位置
autostart=true ; 在 supervisord 启动的时候自动启动
autorestart=true ; 程序异常退出后自动重启
startsecs=5 ; 启动5秒后没有异常退出，就当作已经正常启动
stopasgroup=true ; 停止的时候停止整个进程树
" > ${CHILDREN_CONFIG_GEN_PATH}
  echo "generate config ${CHILDREN_CONFIG_GEN_PATH}"
done

if [ $? -eq 0 ]; then
    echo_green "generate supervisor children conf successfully!"
else
    echo_red "generate supervisor children conf failed!"
fi
