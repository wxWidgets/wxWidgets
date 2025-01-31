#!/bin/bash

cd $(dirname "$0")/../..

rc=0

for h in include/wx/*.h ; do
    header=wx/$(basename "$h")
        if ! grep -q "$header" tests/allheaders.h ; then
            echo "ERROR - <$header> not present in tests/allheaders.h"
            rc=$((rc+1))
        fi
done

exit $rc
