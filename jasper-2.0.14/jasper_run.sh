m -rf fuzz*
mkdir build_dir
mkdir installed
mkdir fuzz_seed
mkdir fuzzing_output
cp $PWD/data/images/test.pnm $PWD/fuzz_seed
export CC="afl-clang"
export CXX="afl-clang++"
cmake -G "Unix Makefiles" -H$PWD -B$PWD/build_dir -DCMAKE_INSTALL_PREFIX=$PWD/installed
cd build_dir
make && make install
cd ..
afl-fuzz -i fuzz_seed -o fuzzing_output -t 300000 -m 200 ./installed/bin/jasper -f @@ -F 1.jp2 -T jp2
