
if [ "$INSTALLDIR" = "" ]; then
  INSTALLDIR=$BUILDDIR/install
  mkdir -p $INSTALLDIR
fi

# wxPython needs this option so that it can build extensions that store the
# right install_name (e.g. /usr/local/lib) even if the files aren't actually
# in /usr/local/lib at the time they're being linked against
# we do this by setting the "official" prefix at build time, and later
if [ "$BUILDPREFIX" = "" ]; then
  BUILDPREFIX=$INSTALLDIR
fi


DEBUG_OPTS="--enable-debug"

UNICODE_OPTS="--enable-unicode"

ANSI_OPTS="--enable-ansi"

WXPYTHON_OPTS="--enable-monolithic \
	--with-opengl \
	--enable-sound \
        --enable-mediactrl \
	--enable-display \
	--enable-geometry \
	--enable-debug_flag \
	--enable-optimise \
	--disable-debugreport "


do_build(){
    OPTS=""
    if [ "$WXPYTHON" == "1" ]; then
      OPTS="$WXPYTHON_OPTS"
    fi

    if [ "$DEBUG" == "1" ]; then
      OPTS="$OPTS $DEBUG_OPTS "
    fi

    if [ "$UNICODE" == "1" ]; then
      OPTS="$OPTS $UNICODE_OPTS "
    fi



    if [ "$STATIC" == "1" ]; then
      OPTS="$OPTS --enable-static"
    fi

    OPTS="$OPTS $OTHER_OPTS"

    $WXROOT/configure --prefix=$BUILDPREFIX $OPTS
    make
}
