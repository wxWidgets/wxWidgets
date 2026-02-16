
case $OSTYPE in
  darwin*)
    PLATFORM="mac"
    export PATH="$PATH:$THIRDPARTY_PATH/cmake/mac/bin"
  ;;  
  *)  
    PLATFORM="linux"
    export PATH="$PATH:$THIRDPARTY_PATH/cmake/lin64/bin"
  ;;  
esac

python3 build/bricsys/build_wx.py "$@" --platform=$PLATFORM --wx_src_dir=.

