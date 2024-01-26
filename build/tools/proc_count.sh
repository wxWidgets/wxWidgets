# This script outputs the number of available processors/cores plus one.

case `uname` in
    Linux|MSYS*|MINGW*)
        wxPROC_COUNT=`nproc`
        ;;

    Darwin|FreeBSD)
        wxPROC_COUNT=`sysctl -n hw.ncpu`
        ;;

    *)
        wxPROC_COUNT=0
        ;;
esac

echo $((wxPROC_COUNT+1))
