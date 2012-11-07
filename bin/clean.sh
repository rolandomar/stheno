#!/bin/bash

find . -name CMakeCache.txt | xargs rm -rf
find . -name Makefile | xargs rm -rf
find . -name CMakeFiles | xargs rm -rf
find . -name cmake_install.cmake | xargs rm -rf
find . -name test | xargs rm -rf
find . -name *.o | xargs rm -rf
find . -name *.so | xargs rm -rf
find . -name *.la | xargs rm -rf
find . -name *.a | xargs rm -rf
find . -name Debug | xargs rm -rf
find . -name Release | xargs rm -rf
find . -name build | xargs rm -rf
find . -name CTestTestfile.cmake | xargs rm -rf
ls * -R | grep -i "~" | xargs rm -rf
