#
# File:		Makefile
# Author:	Julian Smart
# Created:	1998
#
# "%W% %G%"
#
# Makefile : Builds OGL Studio manual
#

DOCDIR=.
LOCALDOCDIR=.

DOCSOURCES=studio.tex

hlp: studio.hlp
html: studio.htm
rtf: studio.rtf

studio.hlp:         studio.rtf studio.hpj
        -erase studio.ph
        hcw /E /C studio.hpj

studio.rtf: $(DOCSOURCES)
        -start /w tex2rtf studio.tex studio.rtf -twice -winhelp

studio.htm: $(DOCSOURCES)
        -start /w tex2rtf studio.tex studio.htm -twice -html

