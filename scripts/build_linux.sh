#!/bin/bash
set -e

if [ "$1" == "" ]; then
  echo "Usage: build_linux Debug|Release"
  exit 1
fi

cd ..
mkdir -p "build/linux/$1/sse2"
cd "build/linux/$1/sse2"
cmake -DCMAKE_BUILD_TYPE="$1" -DFB_ARCH_TYPE=1 ../../../..
make

cd ../../../..
mkdir -p "build/linux/$1/avx2"
cd "build/linux/$1/avx2"
cmake -DCMAKE_BUILD_TYPE="$1" -DFB_ARCH_TYPE=2 ../../../..
make
