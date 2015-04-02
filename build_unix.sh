#!/bin/bash

cur_dir=`dirname "$0"`; cur_dir=`eval "cd \"$cur_dir\" && pwd"`
build_path=$cur_dir"/Build"
install_path=$build_path"/Install"

# build
mkdir $build_path
cd $build_path
cmake -G "Unix Makefiles" ../ -DCMAKE_INSTALL_PREFIX=$install_path
make
make install

echo ""
echo "done !"
read x