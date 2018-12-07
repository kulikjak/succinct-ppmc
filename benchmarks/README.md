This folder contains auxiliary files used for performance measurements.

Included benchmark tests:

1. Performance (both memory and time) of wavelet tree created from individual dynamic
bit vectors (each having its own underlying tree - original structure) and wavelet
tree with binary vectors merged into one common tree (compact structure).

files:
- `structure_common.h` - functions and definitions common to both versions
- `structure_universal.c` - simple multiple tree structure
- `structure_compact.c` - merged compact structure
- `run_structure.sh` - runnable shell script

make targets:
- `structure_compact`
- `structure_universal`
- `clean_strcture` - clean files built for this benchmark
