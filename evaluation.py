#!/bin/python3.6

import os
import sys

# Compressor optimization flags
CACHE_SIZE = "-DCACHE_SIZE"
ENABLE_LOOKUP_CACHE = "-DENABLE_LOOKUP_CACHE"
ENABLE_CACHE_STATS = "-DENABLE_CACHE_STATS"

LABEL_CTX = "-DLABEL_CONTEXT_SHORTENING"
INTEGER_CTX = "-DINTEGER_CONTEXT_SHORTENING"
RAS_CTX = "-DRAS_CONTEXT_SHORTENING"

DIRECT_MEMORY = "-DDIRECT_MEMORY"
INDEXED_MEMORY = "-DINDEXED_MEMORY"

EMBEDED_FLAGS = "-DEMBEDED_FLAGS"
RB_TREE = "-DENABLE_RED_BLACK_BALANCING"

TIME_PROFILING = "-DENABLE_TIME_PROFILING"
MEMORY_PROFILING = "-DENABLE_MEMORY_PROFILING"


compressors = {}


def generate_dna_inputs(sizes, location=""):
    if location:
        os.makedirs(location, exist_ok=True)

    if not os.path.isfile("dnagen"):
        print("Cannot locate dnagen executable", file=sys.stderr)
        sys.exit(1)

    filenames = []
    for size in sizes:
        filename = "{}dna_in{}.txt".format(location, size)
        os.system("./dnagen {} > {}".format(size, filename))
        filenames.append(filename)
    return filenames

def get_compressor(flags):
    if flags in compressors:
        return compressors[flags]

    if flags:
        flags = "CMDFLAGS=\"-DREMOTE_OPTIMIZATION_CONTROL {}\"".format(flags)

    filename = "compressor{}.exe".format(len(compressors))
    res = os.system("make {} compressor".format(flags))
    if res:
        print("Error during compilation")
        sys.exit(2)

    os.rename("compressor", filename)

    compressors[flags] = filename
    return filename


def run_compressor(compressor, file):
    if not os.path.isfile(compressor):
        print("Cannot locate {} executable".format(compressor), file=sys.stderr)
        sys.exit(3)

    res = os.system("./{} -e {} -o {}_{}.out".format(compressor, file, compressor, file))
    if res:
        print("Error during compressor run")
        sys.exit(4)


rand_files = generate_dna_inputs([100, 200, 300])

compressor = get_compressor(" ".join((RB_TREE, INTEGER_CTX, DIRECT_MEMORY)))


for file in files:
    run_compressor(compressor, file)
    print("done {} {}".format(compressor, file))
