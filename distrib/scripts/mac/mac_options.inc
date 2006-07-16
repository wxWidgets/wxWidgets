
UNIVERSAL=0
WXPYTHON=0
UNICODE=0
DEBUG=0
CARBON=1
COCOA=0
STATIC=0

# Process command line options.
for i in "$@"; do
    case "$i" in
	unicode)       UNICODE=1 ;;
	ansi)          UNICODE=0 ;;
	debug)         DEBUG=1 ;;
	wxpython)      WXPYTHON=1 ;;
	universal)     UNIVERSAL=1 ;;
	carbon)        CARBON=1 ;;
	cocoa)         COCOA=1 ;;
	static)        STATIC=1 ;;
	*)
	    usage
	    exit
	    ;;
    esac
done

if [ "$COCOA" = "1" ]; then
  OTHER_OPTS="--with-cocoa "
fi