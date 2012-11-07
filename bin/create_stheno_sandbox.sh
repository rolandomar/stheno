#!/bin/bash
#echo "Project base dir: " $1
#echo "Project dest dir: " $1

mkdir -p $1

cp build/euryale/Debug/medusad $1
cp build/euryale/Debug/*.so $1

cp build/p3/Debug/*.so $1
cp build/p3/Debug/leaf $1
cp build/p3/Debug/monitor $1
cp build/p3/Debug/superpeer $1

cp build/stheno/Debug/*.so $1

echo "Done"


