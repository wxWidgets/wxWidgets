#!/bin/sh
#
# This script is used to check that the ABI of the libraries in the "lib"
# subdirecotry is compatible with the ABI description in the directory
# containing this script itself.

libraries=$(ls -1 lib/*.so)

if [ -z "$libraries" ]; then
    echo 'Please run the script from the build directory and build the libraries first.' >&2
    exit 1
fi

thisdir=$(dirname "$0")

rc=0
for l in $libraries; do
    name=$(basename $l .so)
    echo -n "Checking ${name}... "
    abidiff ${thisdir}/${name}.abi $l
    case $? in
        0)
            echo 'ok'
            ;;

        4)
            echo "*** ABI changes detected in ${name} ***"
            ;;

        *)
            echo "!!! INCOMPATIBLE ABI changes detected in ${name} !!!"
            rc=1
            ;;
    esac
done

exit $rc
