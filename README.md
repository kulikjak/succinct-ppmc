# Succint PPMC algorithm
PPMC compression algorithm using succint deBruijn graphs as an underliying structure.

DeBruijn graph is supported by dynamic succint bit vectors and wavelet tree which support rank and select operations as well as additions (deletion can be supported pretty easily, but it is not needed for PPM compression).
