#!/bin/sh
#

for tname in `ls *.t` 
do
    case $tname in
	b32.t)
	    echo "Generating for Borland C++ (32 bits)..."
	    tmake -t b32 wxwin.pro -o makefile.b32;;

	bcc.t)
	    echo "Generating for Borland C++ (16 bits)..."
	    tmake -t bcc wxwin.pro -o makefile.bcc;;

	dos.t)
	    echo "Generating for Visual C++ 1.52..."
	    tmake -t dos wxwin.pro -o makefile.dos;;

	g95.t)
	    echo "Generating for Cygwin/Mingw32..."
	    tmake -t g95 wxwin.pro -o makefile.g95;;

	sc.t)
	    echo "Generating for Symantec C++..."
	    tmake -t sc wxwin.pro -o makefile.sc;;

	vc.t) 
	    echo "Generating for Visual C++ 4.0..."
	    tmake -t vc wxwin.pro -o makefile.vc;;

	wat.t)
	    echo "Generating for Watcom C++..."
	    tmake -t wat wxwin.pro -o makefile.wat;;

	unx.t)
	    echo "Generating for Unix and Configure..."
	    tmake -t unx wxwin.pro -o Makefile.in;;
    esac
done

