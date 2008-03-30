
UNIVERSAL=0
WXPYTHON=0
UNICODE=0
DEBUG=0
CARBON=1
PORT="mac"
STATIC=0

# Process command line options.
for i in "$@"; do
    case "$i" in
	unicode)       UNICODE=1 ;;
	ansi)          UNICODE=0 ;;
	debug)         DEBUG=1 ;;
	wxpython)      WXPYTHON=1 ;;
	universal)     UNIVERSAL=1 ;;
	carbon)        PORT="mac" ;;
	cocoa)         PORT="cocoa" ;;
	static)        STATIC=1 ;;
	*)             
	    usage
	    exit
	    ;;
    esac
done

OTHER_OPTS="--with-$PORT "