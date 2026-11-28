#!/bin/sh
#
# This script is used to generate the ABI descriptions of the libraries in the
# "lib" subdirectory in the given directory.
#
# It is used by abi_check.yml CI workflow to generate the ABI descriptions of
# the first stable release of the current branch, which are then checked by
# check_all.sh. Note that the ABI descriptions must be generated in the same
# environment as the one used for checking them, to avoid spurious
# differences.

if [ $# -ne 1 ]; then
    echo "Usage: $0 <output-directory>" >&2
    exit 2
fi

outdir="$1"

libraries=$(ls -1 lib/*.so)

if [ -z "$libraries" ]; then
    echo 'Please run the script from the build directory and build the libraries first.' >&2
    exit 1
fi

mkdir -p "$outdir" || exit 1

for l in $libraries; do
    name=$(basename $l .so)
    echo "Generating ABI description of ${name}..."
    abidw --out-file "${outdir}/${name}.abi" $l || exit 1
done
