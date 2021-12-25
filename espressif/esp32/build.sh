#!/bin/bash

set -e

# prebuild #
cd ../../../device/espressif/esp32/sdk_liteos/esp-idf/

CROOT=$(pwd)
OHOS_COMPILE_APP=$CROOT/examples/get-started/hello_world
OHOS_LIBS_DIR=$CROOT/components/harmony/lib
OUTPUT_KERNEL_DIR=$CROOT/../../../../../out/esp32/esp32

rm $OHOS_LIBS_DIR -rf

mkdir -p $OHOS_LIBS_DIR
find $OUTPUT_KERNEL_DIR/libs/ -name '*.a' -exec cp "{}" $OHOS_LIBS_DIR  \;

# build #
. ./export.sh

cd $OHOS_COMPILE_APP
rm $OHOS_COMPILE_APP/build -rf
idf.py clean
idf.py set-target esp32c3
idf.py build
