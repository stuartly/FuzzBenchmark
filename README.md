# VulBenchmark
A collection of benchmarks containing vulnerability for fuzzing

[1] Modification CGC sample for linux, window and mac-os(https://github.com/trailofbits/cb-multios)

[2] LAVA synthetic bug Corpora(http://moyix.blogspot.com/2016/10/the-lava-synthetic-bug-corpora.html)

[3] Google-fuzzer-test-suit (https://github.com/google/fuzzer-test-suite)



# Submit Format

Please submit a folder named with project name and its version (e.g bision-3.0.4), and the folder should contains below things:
 + Download Link, which is a link for downloading the target project.
 + Fuzz_seed, which is folder containing the seed files used for fuzzing
 + FuzzScript.sh, which is a script file used for fuzzing specific executable binary
 + CrashDir, which is a folder containing the test cases that could crash the application


