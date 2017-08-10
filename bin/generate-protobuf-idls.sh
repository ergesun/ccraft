#!/bin/bash

BIN_DIR=$(cd `dirname $0`; pwd)
CMAKELISTS_ROOT_DIR=${BIN_DIR}/..

PROTOBUF_IDL_DIR_PATH=${CMAKELISTS_ROOT_DIR}/protobuf
THIRDPARTY_BIN_PATH=${CMAKELISTS_ROOT_DIR}/third_party/bin

LANG=cpp
OUT_DIR="/tmp"

# load color utils
. ${BIN_DIR}/utils/color-utils.sh

function print_usage {
  echo_red "Usage: generate thrift IDLs COMMAND."
  echo_yellow "where COMMAND is one of:"
  echo_yellow "  -l \t gen language(cpp、java and so on)."
  echo_yellow "  -o \t output path."
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

mkdir -p ${OUT_DIR}

echo_yellow "generating..."
echo " "

HAS_WARN=

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${CMAKELISTS_ROOT_DIR}/third_party/lib
for f in $(ls ${CMAKELISTS_ROOT_DIR}/protobuf)
do
    echo ${CMAKELISTS_ROOT_DIR}/protobuf/${f}
    ${THIRDPARTY_BIN_PATH}/protoc -I${CMAKELISTS_ROOT_DIR}/protobuf --${LANG}_out=${OUT_DIR} ${CMAKELISTS_ROOT_DIR}/protobuf/${f}
    if [ "$?" -ne 0 ]; then
        echo_red "ERROR: gen error."
        exit 1
    fi
done

if [ "$LANG" == "cpp" ]; then
    cp ${BIN_DIR}/resources/protobuf-gen-cmakelists.txt ${OUT_DIR}/CMakeLists.txt
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
