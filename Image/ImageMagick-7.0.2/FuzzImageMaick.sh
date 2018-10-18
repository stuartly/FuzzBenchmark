git clone https://github.com/ImageMagick/ImageMagick.git
cd ImageMagick

#install
CC=afl-clang-fast ./configure --prefix=$PWD/installed
make -j5
make install


#Prepare seed
afl-fuzz -i fuzz_seed -o output -t 300000 -m 200 ./magick convert @@ /dev/null
