#!/bin/bash -eu

# build project
./configure --without-subdirs --disable-shared --disable-sys-libs --disable-gui LDFLAGS="$CXXFLAGS"
make -j$(nproc)

# build fuzzers
$CXX $CXXFLAGS -o $OUT/zip tests/fuzz/zip.cpp \
    $LIB_FUZZING_ENGINE `./wx-config --cxxflags --libs base`

# and copy their corpora
zip -j $OUT/zip_seed_corpus.zip tests/fuzz/corpus/zip/*
