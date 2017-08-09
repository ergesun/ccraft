#!/bin/bash

BIN_DIR=$(cd `dirname $0`; pwd)
CMAKELISTS_ROOT_DIR=${BIN_DIR}/..

LANG=cpp
OUT_DIR="/tmp"

# load color utils
. ${BIN_DIR}/utils/color-utils.sh

function print_usage {
  echo_red "Usage: generate thrift IDLs COMMAND."
  echo_yellow "where COMMAND is one of:"
  echo_yellow "  -l \t gen language(cpp、java and so on)."
  echo_yellow "  -o \t output path(refer to \"thrift -o)\"."
  echo_yellow "  -h \t show help."
}

# parse params
if [ 0 -lt $# ]; then
    declare -i m=$(($#%2))
    if [ $m -ne 0 ]; then
        print_usage
        exit 1
    else
        declare -i cType=0
        for c in $*
        do
            if [ $cType -eq 0 ]; then
                cType=1
                lastCmd=$c
                if [ $c != "-l" -a $c != "-o" ]; then
                    lastCmd="error cmd!"
                    print_usage
                    exit 1
                fi
            else
                cType=0
                case $lastCmd in
                  -l)
                      LANG=$c
                      ;;
                  -o)
                      OUT_DIR=$c
                      ;;
                  *)
                      print_usage
                      exit 1
                      ;;
                esac
            fi
        done
    fi
else
    print_usage
    exit 1
fi

echo_yellow "-------------"
echo_yellow "|config info|"
echo_yellow "-------------"

echo_pink "language is \"${LANG}\""
echo_pink "output dir is \"${OUT_DIR}\""

echo_green "------------------------------------------------------------------------------------"

echo_yellow "generating..."
echo " "

HAS_WARN=

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${CMAKELISTS_ROOT_DIR}/third_party/lib
for f in $(ls ${CMAKELISTS_ROOT_DIR}/be/thrift-idls)
do
    #thrift --gen ${LANG} -o ${CMAKELISTS_ROOT_DIR}/be/src ${CMAKELISTS_ROOT_DIR}/be/thrift-idls/${f}
    thrift --gen ${LANG} -o ${OUT_DIR} ${CMAKELISTS_ROOT_DIR}/be/thrift-idls/${f}
    if [ "$?" -ne 0 ]; then
        echo_red "ERROR: gen error."
        exit 1
    fi
done

# 清除thrift自动生成的样例server以及handler文件。
for f in $(ls ${CMAKELISTS_ROOT_DIR}/be/src/gen-cpp|grep "skeleton")
do
    SKELETON=${CMAKELISTS_ROOT_DIR}/be/src/gen-cpp/${f}
    rm ${SKELETON}
    if [ "$?" -ne 0 ]; then
        echo_yellow "WARN: rm skeleton ${SKELETON} error."
        HAS_WARN="1"
    fi
done

if [ "$LANG" == "cpp" ]; then
    cp ${BIN_DIR}/conf/thrift-gen-cmakelists.txt ${CMAKELISTS_ROOT_DIR}/be/src/gen-cpp/CMakeLists.txt
    if [ "$?" -ne 0 ]; then
        echo_yellow "WARN: cp CMakeLists.txt error."
        HAS_WARN="1"
    fi
fi

if [ "$HAS_WARN" != "" ]; then
    echo_yellow "gen successfully but has something err occur!"
else
    echo_green "gen successfully!"
fi
