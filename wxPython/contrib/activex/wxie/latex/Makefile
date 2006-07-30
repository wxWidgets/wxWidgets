all: refman.dvi

ps: refman.ps

pdf: refman.pdf

ps_2on1: refman_2on1.ps

pdf_2on1: refman_2on1.pdf

refman.ps: refman.dvi
	dvips -o refman.ps refman.dvi

refman.pdf: refman.ps
	gswin32c -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=refman.pdf -c save pop -f refman.ps

refman.dvi: refman.tex doxygen.sty
	echo "Running latex..."
	latex refman.tex
	echo "Running makeindex..."
	makeindex refman.idx
	echo "Rerunning latex...."
	latex refman.tex
	latex_count=5 ; \
	while egrep -s 'Rerun (LaTeX|to get cross-references right)' refman.log && [ $$latex_count -gt 0 ] ;\
	    do \
	      echo "Rerunning latex...." ;\
	      latex refman.tex ;\
	      latex_count=`expr $$latex_count - 1` ;\
	    done

refman_2on1.ps: refman.ps
	psnup -2 refman.ps >refman_2on1.ps

refman_2on1.pdf: refman_2on1.ps
	gswin32c -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=refman_2on1.pdf -c save pop -f refman_2on1.ps

clean:
	rm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out
