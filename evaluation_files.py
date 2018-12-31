#!/usr/bin/python3.6

import os
import sys
import getopt
import subprocess

from typing import List, Set, Dict, TextIO

# Compressor optimization flags
CACHE_SIZE = "-DCACHE_SIZE"
ENABLE_LOOKUP_CACHE = "-DENABLE_LOOKUP_CACHE"
ENABLE_CACHE_STATS = "-DENABLE_CACHE_STATS"

LABEL_CTX = "-DLABEL_CONTEXT_SHORTENING"
INTEGER_CTX = "-DINTEGER_CONTEXT_SHORTENING"
RAS_CTX = "-DRAS_CONTEXT_SHORTENING"

SIMPLE_MEMORY = "-DSIMPLE_MEMORY"
DIRECT_MEMORY = "-DDIRECT_MEMORY"
INDEXED_MEMORY = "-DINDEXED_MEMORY"

FREQ_INC_NONE = "-DFREQ_INCREASE_NONE"
FREQ_INC_FIRST = "-DFREQ_INCREASE_FIRST"
FREQ_INC_ALL = "-DFREQ_INCREASE_ALL"

FREQ_CNT_EACH = "-DFREQ_COUNT_EACH"
FREQ_CNT_ONCE = "-DFREQ_COUNT_ONCE"

FAST_RANK = "-DFAST_RANK"
FAST_SELECT = "-DFAST_SELECT"

EMBEDED_FLAGS = "-DEMBEDED_FLAGS"
RB_TREE = "-DENABLE_RED_BLACK_BALANCING"

TIME_PROFILING = "-DENABLE_TIME_PROFILING"
MEMORY_PROFILING = "-DENABLE_MEMORY_PROFILING"

input_files: Dict[int, str] = {}
compressors: Dict[str, str] = {}
output_files: Set[str] = set()

# All results of all applications
results: Dict = {}


vprint = lambda *a, **k: None

def cleanup() -> None:
    for file in input_files.values():
        os.remove(file)

    for file in compressors.values():
        os.remove(file)

    for file in output_files:
        os.remove(file)


def get_compressor(flags: str, location: str = "") -> str:
    xflags = ""

    # return cached compressor file if one exists
    if flags in compressors:
        return compressors[flags]

    # setup flags passed to the Makefile
    if flags:
        xflags = f"CMDFLAGS=\"-DREMOTE_OPTIMIZATION_CONTROL {flags}\""

    if location:
        os.makedirs(location, exist_ok=True)

    # remove already existing compressor executable if one exists
    if os.path.isfile("compressor"):
        os.rename("compressor", "compressor_old")

    vprint(f"Compiling compressor with following the flags: {flags}")
    # create compressor, rename it and return its path
    filename = f"{location}compressor{len(compressors)}.exe"
    res = subprocess.run(f"make {xflags} compressor",
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE,
                         shell=True)
    if res.returncode:
        print("Error during compilation")
        print(res.stderr.decode("utf8"))
        cleanup()
        sys.exit(4)

    os.rename("compressor", filename)

    compressors[flags] = filename
    return filename


def run_compressor(compressor: str, file: str) -> str:
    if not os.path.isfile(compressor):
        print(f"Cannot locate {compressor} executable", file=sys.stderr)
        cleanup()
        sys.exit(2)

    vprint(f"running {compressor} with file {file}")
    filename = f"{compressor}_{file}.out"
    output_files.add(filename)
    res = subprocess.run(f"./{compressor} -e {file} -o {filename}",
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE,
                         shell=True)
    if res.returncode:
        print("Error during compressor run")
        print(res.stderr.decode("utf8"))
        cleanup()
        sys.exit(5)

    # Get list line containing all the profiling information
    output = res.stdout.decode("utf8")
    lline = output.split()[-1]

    return lline


def memory_test(flags: List[str], file: str) -> str:
    # check whether the memory profiling flag is already in flags
    if MEMORY_PROFILING in flags:
        vprint("Memory profiling flag already in flags")
    else:
        flags = flags + [MEMORY_PROFILING]

    # check if both are enabled at the same time
    if TIME_PROFILING in flags:
        print("Cannot run with both time and memory profiling enabled at the same time",
              file=sys.stderr)
        cleanup()
        sys.exit(10)

    # Sort flags for results lookup
    flags.sort()

    # Check if results already exist
    key = "".join(flags + [file])
    if key in results:
        vprint("Using cached results")
        return results[key]

    compressor = get_compressor(" ".join(flags))
    res = run_compressor(compressor, file)

    results[key] = res
    return res


def time_test(flags: List[str], file: str, runs: int = 5) -> List[str]:
    # check whether the time profiling flag is already in flags
    if TIME_PROFILING in flags:
        vprint("Time profiling flag already in flags")
    else:
        flags = flags + [TIME_PROFILING]

    # check if both are enabled at the same time
    if MEMORY_PROFILING in flags:
        print("Cannot run with both time and memory profiling enabled at the same time",
              file=sys.stderr)
        cleanup()
        sys.exit(10)

    # Sort flags for results lookup
    flags.sort()

    # Check if results already exist
    key = "".join(flags + [file])
    if key in results:
        vprint("Using cached results")
        return results[key]

    measurements = []
    compressor = get_compressor(" ".join(flags))
    for _ in range(runs):
        res = run_compressor(compressor, file)
        measurements.append(res)

    results[key] = measurements
    return measurements


def get_file_size(filename: str) -> int:
    if not os.path.isfile(filename):
        print(f"Cannot locate file {filename}", file=sys.stderr)
        cleanup()
        sys.exit(2)

    return os.path.getsize(filename)


def run_time_test(ofile: TextIO, test_files: List[str], dflags: List[str], sflags: List[str]) -> None:
    for test_file in test_files:
        ofile.write(f"{test_file}")

        for flag in sflags:
            # concatenate default flags and changing one and create string from them
            time = time_test(dflags + [flag], test_file)
            avg = sum(float(item) for item in time) / len(time)
            ofile.write(f" {avg:.6f}")
        ofile.write("\n")


def run_memory_test(ofile: TextIO, test_files: List[str], dflags: List[str], sflags: List[str]) -> None:
    for test_file in test_files:
        ofile.write(f"{test_file}")

        for flag in sflags:
            # concatenate default flags and changing one and create string from them
            memory = memory_test(dflags + [flag], test_file)
            split = memory.split(":")
            ofile.write(f" {split[-2]} {split[-1]}")
        ofile.write("\n")


def run_ratio_test(ofile: TextIO, test_files: List[str], dflags: List[str], sflags: List[str]) -> None:
    for test_file in test_files:
        ofile.write(f"{test_file}")

        for flag in sflags:
            # Run time test and discard the result
            _ = memory_test(dflags + [flag], test_file)
            # Get cached file from time profiling stage
            flags = dflags + [flag, MEMORY_PROFILING]
            flags.sort()
            compressor = get_compressor(" ".join(flags))

            filename = f"{compressor}_{test_file}.out"
            size = get_file_size(filename)
            ofile.write(f" {size}")
        ofile.write("\n")


def print_compressor_list() -> None:
    for flags, compressor in compressors.items():
        print(f"{compressor} > {flags}")


def main() -> None:

    def usage(err: str) -> None:
        print(err, file=sys.stderr)
        print(f"Usage:\n{sys.argv[0]} [-v|--verbose] <test files>", file=sys.stderr)

    def dupe_flags(default: List[str], without: List[str] = None) -> List[str]:
        flags = default[:]
        if without is not None:
            for item in without:
                if item in flags:
                    flags.remove(item)
        return flags

    try:
        opts, args = getopt.getopt(sys.argv[1:], "v", ["verbose"])
    except getopt.GetoptError as err:
        usage(str(err))
        sys.exit(1)

    for opt, _ in opts:
        if opt in ["-v", "--verbose"]:
            global vprint
            vprint = print
        else:
            assert False, "unknown option"

    test_files: List[str] = []
    for file in args:
        if not os.path.isfile(file):
            print(f"Cannot find given file {file}", file=sys.stderr)
            sys.exit(100)

        test_files.append(file)

    os.makedirs("resultsx", exist_ok=True)
    # create list of default flags which are being used in each test
    default_flags = [RB_TREE, EMBEDED_FLAGS, INTEGER_CTX, SIMPLE_MEMORY,
                     FREQ_INC_ALL, FREQ_CNT_ONCE, FAST_RANK, FAST_SELECT]

    def test_zero():
        # TEST 0: Compression ratio based on escape handeling
        vprint("Running test 0")
        flags = dupe_flags(default_flags, without=[FREQ_CNT_ONCE, FREQ_CNT_EACH])
        with open("resultsx/test0.txt", "w") as file:
            file.write("size once each\n")
            run_ratio_test(file, test_files, flags,
                           [FREQ_CNT_ONCE, FREQ_CNT_EACH])

    def test_one():
        # TEST 1: Compression ratio based on frequency increases
        vprint("Running test 1")
        flags = dupe_flags(default_flags, without=[FREQ_INC_NONE, FREQ_INC_FIRST, FREQ_INC_ALL])
        with open("resultsx/test1.txt", "w") as file:
            file.write("size none first all\n")
            run_ratio_test(file, test_files, flags,
                           [FREQ_INC_NONE, FREQ_INC_FIRST, FREQ_INC_ALL])

    def test_four():
        # TEST 4: Cache sizes time performance
        vprint("Running test 4")
        with open("resultsx/test4.txt", "w") as file:
            file.write("size cache sizes 0...4\n")
            run_time_test(file, test_files, default_flags,
                          [f"-DCACHE_SIZE={i}" for i in range(0, 5)])

    def test_five():
        # TEST 5: Context length comparison (time, memory and compression ratio)
        vprint("Running test 5")
        with open("resultsx/test5_time.txt", "w") as file:
            file.write("size context sizes 2...8, +2\n")
            run_time_test(file, test_files, default_flags,
                          [f"-DCONTEXT_LENGTH={i}" for i in range(2, 9, 2)])

        with open("resultsx/test5_memory.txt", "w") as file:
            file.write("size context sizes 2...8, +2\n")
            run_memory_test(file, test_files, default_flags,
                            [f"-DCONTEXT_LENGTH={i}" for i in range(2, 9, 2)])

        with open("resultsx/test5_ratio.txt", "w") as file:
            file.write("size context sizes 2...8, +2\n")
            run_ratio_test(file, test_files, default_flags,
                           [f"-DCONTEXT_LENGTH={i}" for i in range(2, 9, 2)])

    def test_eight():
        # TEST 8: Context shortening and context length correlation
        vprint("Running test 8")
        flags = dupe_flags(default_flags, without=[LABEL_CTX, INTEGER_CTX, RAS_CTX])
        with open("resultsx/test8.txt", "w") as file:
            file.write("size label 0..8 +2, integer 0..8 +2\n")
            run_time_test(file, test_files, flags,
                          [f"{LABEL_CTX} -DCONTEXT_LENGTH={i}" for i in range(0, 9, 2)] + \
                          [f"{INTEGER_CTX} -DCONTEXT_LENGTH={i}" for i in range(0, 9, 2)])

    test_zero()
    test_one()
    test_four()
    test_five()
    test_eight()

    # Tests not important for huge genome non-random files

    # TEST 2: Red black balancing time improvements
    # TEST 3: Memory based on different context shortening algorithm
    # TEST 6: Memory model time performance and memory usage
    # TEST 7: Memory and time changes based on embeded and non-embeded flags
    # TEST 9: Fast rank and select time performance

    cleanup()
    sys.exit(0)

if __name__ == '__main__':
    main()
