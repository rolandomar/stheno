#!/bin/bash
echo "main:"
hg add

echo "euryale:"
cd euryale
hg add
echo ""

echo "stheno:"
cd ../stheno
hg add
echo ""

echo "p3:"
cd ../p3
hg add



