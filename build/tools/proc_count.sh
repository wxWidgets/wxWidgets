# This script outputs the number of available processors/cores plus one.

case `uname` in
    Linux)
        wxPROC_COUNT=`nproc`
        ;;

    Darwin|FreeBSD)
        wxPROC_COUNT=`sysctl -n hw.ncpu`
        ;;

    *)
        wxPROC_COUNT=0
        ;;
esac

((wxPROC_COUNT++))
echo $wxPROC_COUNT
