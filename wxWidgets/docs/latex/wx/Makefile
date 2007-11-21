FIGS_GIF = \
	sizer00.gif sizer01.gif sizer02.gif sizer03.gif sizer04.gif \
	sizer05.gif sizer06.gif sizer07.gif sizer08.gif sizer09.gif \
	sizer10.gif sizer11.gif

FIGS_EPS = $(FIGS_GIF:.gif=.eps)
LATEX = hugelatex
MAKEINDEX = makeindex
DVIPS = dvips

all: manual.ps

manual.ps: manual.dvi
	$(DVIPS) $< -o $@

manual.dvi: manual.tex $(FIGS_EPS)
	$(LATEX) manual.tex
	$(MAKEINDEX) -s manual.ist manual
	$(LATEX) manual.tex
	$(MAKEINDEX) -s manual.ist manual
	$(LATEX) manual.tex


$(FIGS_EPS): %.eps : %.gif
	giftopnm $< | pnmtops -noturn > $@

clean:
	-rm -f *.dvi *.aux *.idx *.ilg *.ind *.log *.toc $(FIGS_EPS) *~