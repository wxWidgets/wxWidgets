echo "dsp2dos [ver. 1.1]"
echo "Convert wxWidgets workspaces from Unix to Dos format"
echo "  Author : Marco Cavallini (www.koansoftware.com)"
echo "You need to build wxWidgets/distrib/msw/unix2dos.c for using it"
unix2dos --unix2dos $WXWIN/src/wxBase.dsp
unix2dos --unix2dos $WXWIN/src/wxBase.dsw
unix2dos --unix2dos $WXWIN/src/wxUniv.dsp
unix2dos --unix2dos $WXWIN/src/wxUniv.dsw
unix2dos --unix2dos $WXWIN/src/wxWindows.dsp
unix2dos --unix2dos $WXWIN/src/wxWindows.dsw
unix2dos --unix2dos $WXWIN/src/jpeg/jpeg.dsp
unix2dos --unix2dos $WXWIN/src/jpeg/jpeg.dsw
unix2dos --unix2dos $WXWIN/src/png/png.dsp
unix2dos --unix2dos $WXWIN/src/png/png.dsw
unix2dos --unix2dos $WXWIN/src/regex/regex.dsp
unix2dos --unix2dos $WXWIN/src/regex/regex.dsw
unix2dos --unix2dos $WXWIN/src/tiff/tiff.dsp
unix2dos --unix2dos $WXWIN/src/tiff/tiff.dsw
unix2dos --unix2dos $WXWIN/src/zlib/zlib.dsp
unix2dos --unix2dos $WXWIN/src/zlib/zlib.dsw
unix2dos --unix2dos $WXWIN/src/msw/wince/wxBase.vcp
unix2dos --unix2dos $WXWIN/src/msw/wince/wxBase.vcw
unix2dos --unix2dos $WXWIN/src/msw/wince/wxWinCE.vcp
unix2dos --unix2dos $WXWIN/src/msw/wince/wxWinCE.vcw
unix2dos --unix2dos $WXWIN/src/msw/wince/mywxbase.vcp
unix2dos --unix2dos $WXWIN/src/msw/wince/mywxbase.vcw
echo "Done"
echo ""
