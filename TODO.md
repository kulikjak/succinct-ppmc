# Todo list

Implementation and possible text todo list for this thesis

## Simple version:
* red black tree support
* more optimal indel array ?


## Compact version:
* red black tree support - done
* rank support - done
* select support - done
* de Bruijn graph operations - done
* compression of given text - done
* arithmetic coding output of compression - done

* write more and better tests
* add tests for P array - done

* decompression of compressed text - done
* extensive tests for compression and decompression

* code refactoring and nicening
* optimizations of algorithms

* benchmarks for all versions

* fix rank and select correct values when pos is bigger than size of the vector
* implement PPMD for testing purposes with 7zip
	- Image and Text Compression (Storer)

* move code tracker from compressor/decompressor
* memory should be global and not associated with stucture
* change several functions to macros
* code deduplication

## Text (among all other):

* removal of third vector
* removal of pointers in graph structure
* compression vs memory usage (size of P values)

* info about existing SDSL library
