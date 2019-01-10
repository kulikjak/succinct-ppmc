# Succinct PPMC compressor
PPMC compression algorithm using succinct de Bruijn graphs as an
underlying structure. This prototype version is used to compress
dna strings and is unable to work with general alphabet.

This compressor aims to have compression ratio comparable to other
widely used compressors while using much less memory. That is achieved
by employing succinct data structures, which are in their dynamic
form slower (logarithmic) but also very small.

The project uses a single tree for all the vectors and wavelet
tree which greatly reduces the time needed to query it and allows
us to do more in less time.

*This compressor is a result of Master's thesis.*

While this project is based on the PPMC method, it is not directly
compatible with other such compressors as the graph-based approach
used here cannot simulate behavior of ppm trees.

## Branches
This project contains several branches with different compressors:

### master
Main branch with dna compression capable program. Not much more to
say here...

### dollar_free
This branch includes compressor similar to the **master** except
that it doesn't use additional wavelet tree vector for storing dollar
symbol. This makes the whole tree more balanced and without memory
overhead by the vectors.

### tree_compressor
Compressor based on the trees (and XBW transformation) rather than
graphs (which is a standard way for ppm). It is an early compressor
prototype and doesn't contain later improvements meaning, that a lot
can be upgraded there. Compared to graph-based compressors it uses
much more memory as the number of nodes in the graph is bigger, it
is slower because of the same reason, but it can achieve better
compression ratios due to the more information that is being stored.

While some functions and structures are named de Bruijn, they are
not really representing graphs and are only named there because
they were.

## Build and Run
Building compressor with GNU make:

    gmake compressor

Running unit tests for the compressor and its structures:

    cd tests
    gmake all

Running compressor in the compression mode:

	./compressor -e input.in -o compressed.out

Running compressor in the decompression mode:

	./compressor -d compressed.in -o result.out

## Note

folder **src/arith** contains several files from:
https://people.eng.unimelb.edu.au/ammoffat/arith_coder/

Those files are not 100% same as the original as they were updated
for the purpose of this compression program.  The whole program with
original files can be found on the website given above.

For more information about the code and its license check the file
**/src/arith/NOTE** or the website given above.
