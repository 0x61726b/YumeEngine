#!/bin/sh
set -ex
wget --no-check-certificate http://cmake.org/files/v3.2/cmake-3.2.2-Linux-x86_64.tar.gz
tar -xzvf cmake-3.2.2-Linux-x86_64.tar.gz
export PATH=$(pwd)/cmake-3.2.2-Linux-x86_64/bin:$PATH
cmake --version
