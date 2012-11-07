#!/bin/bash
echo "main:"
hg st

echo "euryale:"
cd euryale
hg st
echo ""

echo "stheno:"
cd ../stheno
hg st
echo ""

echo "p3:"
cd ../p3
hg st


