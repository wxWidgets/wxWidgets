#
# File:		Makefile
# Author:	Julian Smart
# Created:	1999
#
# Makefile : Builds Tex2RTF manual
#

DOCDIR=..
LOCALDOCDIR=.

DOCSOURCES=tex2rtf.tex

all: tex2rtf.htb

hlp: tex2rtf.hlp
htb: tex2rtf.htb
html: tex2rtf.html
htmlhelp: tex2rtf.chm
pdfrtf: tex2rtf_rtf.rtf

tex2rtf_rtf.rtf: $(DOCSOURCES) tex2rtf.ini
	tex2rtf tex2rtf.tex tex2rtf_rtf.rtf -twice -rtf

tex2rtf.rtf: $(DOCSOURCES) tex2rtf.ini
	tex2rtf tex2rtf.tex tex2rtf.rtf -twice -winhelp

tex2rtf_contents.html:         $(DOCSOURCES) tex2rtf.ini
	tex2rtf tex2rtf.tex tex2rtf.html -twice -html
	rm -f *.con
	rm -f *.ref
	rm -f *.con
	rm -f *.ref

tex2rtf.htb:         tex2rtf_contents.html
	zip tex2rtf.htb tex2rtf*.html *.hhp *.hhk *.hhc *.gif
