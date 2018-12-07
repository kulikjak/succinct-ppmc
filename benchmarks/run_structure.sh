#!/bin/bash

STRUCTURE_SAMPLE_SIZE=10000
STRUCTURE_SECONDARY_SIZE=10000
clean=false

echo \
"This benchmark test compares wavelet tree created from individual dynamic bit
vectors (each having its own underlying tree - original structure) with wavelet
tree created from binary vectors merged into one common tree (compact structure)."
echo

if [ ! -f "structure_universal" ] ||
   [ ! -f "structure_compact" ]; then
  echo "Needed executables not found - trying to build them."
  make structure_
  if [ $? -ne 0 ]; then
    echo "Build ended with errors."
    make clean_structure
    exit 1
  fi
  clean=true
fi

echo "[Sample size]:"
echo "Vector size for all tests is $STRUCTURE_SAMPLE_SIZE."
echo
echo "[Secondary size]:"
echo "Random rank|select|access tests do $STRUCTURE_SECONDARY_SIZE operations each."

echo
echo "================= Original structure ================"
./structure_universal "$STRUCTURE_SAMPLE_SIZE" "$STRUCTURE_SECONDARY_SIZE"

echo
echo "================= Compact structure ================="
./structure_compact "$STRUCTURE_SAMPLE_SIZE" "$STRUCTURE_SECONDARY_SIZE"


if [ "$clean" = true ]; then
  make clean_structure
fi
