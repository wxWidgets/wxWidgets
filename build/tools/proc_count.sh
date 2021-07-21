# This script is sourced by CI scripts to return the number of available
# processors/cores plus one.

proc_count() {
    case `uname` in
        Linux)
            wxPROC_COUNT=`nproc`
            ;;

        Darwin)
            wxPROC_COUNT=`sysctl -n hw.ncpu`
            ;;

        *)
            wxPROC_COUNT=0
            ;;
    esac

    ((wxPROC_COUNT++))
    echo $wxPROC_COUNT
}
