git clone https://github.com/Distrotech/bison.git

# install
cd bison
CC=afl-clang-fast ./configure --prefix=$PWD/installed
make -j5
make install

afl-fuzz -i fuzz_seed -o fuzz_out ./installed/bin/bison @@
