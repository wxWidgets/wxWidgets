#
# File:		Makefile
# Author:	Julian Smart
# Created:	1999
#
# Makefile : Builds wxWindows configuration tool manual
#

DOCDIR=..
LOCALDOCDIR=.

DOCSOURCES=configtool.tex

all: configtool.chm configtool.htb

hlp: configtool.hlp
htb: configtool.htb
html: configtool.html
htmlhelp: configtool.chm
pdfrtf: configtool_rtf.rtf

configtool.hlp:	configtool.rtf configtool.hpj
	rm -f configtool.ph
	-hcw /E /C configtool.hpj

configtool.chm : configtool.html configtool.hhp
	-hhc configtool.hhp
#    -mkdir ..\htmlhelp

configtool_rtf.rtf: $(DOCSOURCES) tex2rtf.ini
	-tex2rtf configtool.tex configtool_rtf.rtf -twice -rtf

configtool.rtf: $(DOCSOURCES) tex2rtf.ini
	-tex2rtf configtool.tex configtool.rtf -twice -winhelp

configtool.html:         $(DOCSOURCES) tex2rtf.ini
	-tex2rtf configtool.tex configtool.html -twice -html
	-rm -f *.con
	-rm -f *.ref
	-rm -f *.con
	-rm -f *.ref
#	-tweakmanual

configtool.htb:         configtool_contents.html
	-zip32 configtool.htb configtool*.html *.hhp *.hhk *.hhc *.gif

