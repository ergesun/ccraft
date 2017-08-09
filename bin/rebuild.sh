#!/bin/bash

BIN_DIR=$(cd `dirname $0`; pwd)

${BIN_DIR}/clean.sh
${BIN_DIR}/build.sh $@
