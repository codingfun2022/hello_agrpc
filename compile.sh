#!/bin/bash

set -e

if [[ ! -f ~/.vcpkg/scripts/buildsystems/vcpkg.cmake ]]
then
    git clone https://github.com/Microsoft/vcpkg.git ~/.vcpkg
    ~/.vcpkg/bootstrap-vcpkg.sh
fi

pushd $(dirname ${BASH_SOURCE[0]})
rm -rf build
rm -rf built
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=built -H. -Bbuild -DCMAKE_TOOLCHAIN_FILE=~/.vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
popd
echo OK
