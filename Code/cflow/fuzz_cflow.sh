https://www.gnu.org/software/cflow/#downloading

export CC=afl-clang-fast
export CXX/afl-clang-fast++
./configure --prefix=$PWD/installed
make -j5
make install
make clean

afl-fuzz -i fuzz_seed -o fuzz_out ./installed/bin/cflow -a @@
