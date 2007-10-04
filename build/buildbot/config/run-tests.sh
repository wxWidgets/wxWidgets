# Run the test suites
# Running them one at a time gives more readable results and shows up errors
# in the suite names.
set -e
cd tests

case `uname -sm` in
    CYGWIN*|MINGW32*)   PATH=../lib:$PATH ;;
    Darwin*)            DYLD_LIBRARY_PATH=../lib:$DYLD_LIBRARY_PATH ;;
    *)                  LD_LIBRARY_PATH=../lib:$LD_LIBRARY_PATH ;;
esac

test -x test_gui && TEST_GUI=./test_gui
ERR=0

for prog in ./test $TEST_GUI; do
    if [ -x $prog ]; then
        SUITES=`$prog -l | grep '^  [^ ]'`
        for SUITE in $SUITES; do
            echo
            echo $SUITE
            echo $SUITE | sed 's/./-/g'
            $prog $SUITE || ERR=1
            echo
        done
    fi
done

exit $ERR
