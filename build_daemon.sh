#!/bin/sh

if test -z $ANDROID_NDK_HOME; then
    echo "Could not find Android NDK, set \$ANDROID_NDK_HOME and try again"
    exit 1
fi

ndk_build="$ANDROID_NDK_HOME/ndk-build"
script_dir="$(dirname $0)"
builddir="$PWD"

pushd "$script_dir/nandroidfs/nandroid_daemon" > /dev/null

$ndk_build "NDK_PROJECT_PATH=$PWD" "APP_BUILD_SCRIPT=$PWD/Android.mk" "NDK_APPLICATION_MK=$PWD/Application.mk" "NDK_LIBS_OUT=$builddir/daemon/lib" "NDK_OUT=$builddir/daemon/obj" "TARGET_OUT=$builddir"

popd > /dev/null

