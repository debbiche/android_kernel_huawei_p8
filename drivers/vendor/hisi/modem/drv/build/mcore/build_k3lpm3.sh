#!/bin/bash 
set -e

ANDROID_ROOT_DIR=$1
ANDROID_PRODUCT_NAME=$2
ANDROID_ARCH=$3


cd ${ANDROID_ROOT_DIR}

. build/envsetup.sh

if [ "${ANDROID_ARCH}" = "arm64" ]; then
    choosecombo release ${ANDROID_PRODUCT_NAME} eng normal fullAP arm64
else
    choosecombo release ${ANDROID_PRODUCT_NAME} eng normal 
fi

./prebuilts/misc/linux-x86/ccache/ccache -M 50G
android_pkgs="seclpm3image"
echo "make  -j 48 " ${android_pkgs}
make  -j 48 ${android_pkgs}

exit $?
