Generating the FL Reference Manual
===================================

Like the wxWindows manual, the FL manual uses Tex2RTF to
generate HTML, MS HTML Help, wxHTML Help, WinHelp, RTF, and PDF
formats.

VC++ users can use makefile.vc in this directory, e.g.:

  nmake -f makefile.vc docs      ; Create directories and
                                 ; all formats

or

  nmake -f makefile.vc dirs html ; Create directories, then
                                 ; HTML files

The targets that the makefile supports are:

html        ; HTML
htb         ; wxHTML Help: the HTML files + hhp, hhc files,
            ; archived in a fl.htb (zip format) file
htmlhelp    ; MS HTML Help: the HTML files + hhp, hhc files
            ; then processed with hhc to produce fl.chm
winhelp     ; WinHelp format: Tex2RTF produces RTF and CNT
            ; files, then the makefile invokes hc.exe to produce
            ; the fl.hlp file
rtf:        ; WinHelp RTF (intermediate stage for winhelp)
pdfrtf      ; Makes an RTF file suitable for loading into
            ; MS Word and generating a PDF via Adobe Acrobat

If you wish to run Tex2RTF directly, you can do e.g.

  tex2rtf $(DOCDIR)\latex\FL\manual.tex $(DOCDIR)\html\FL\fl.htm -twice -html

For more information on using Tex2RTF, please see:

  docs/tech/tn0003.txt: Adding wxWindows class documentation

in the wxWindows hierarchy, and also utils/tex2rtf. The Tex2RTF
manual is supplied with wxWindows documentation, in (for example)
docs/html/tex2rtf/t2rtf.htm or docs/htmlhelp/tex2rtf.chm.

You can configure the way Tex2RTF works with tex2rtf.ini in
docs/latex/fl.

Generating HTML format
======================

To generate HTML documentation, you need to use the -html
argument to Tex2RTF. A whole directory of files will be
generated.

Generating MS HTML Help format
==============================

To generate MS HTML Help documentation, you need to generate
the HTML files as above and then run

  hhc fl.hhp

You'll need the MS HTML Help compiler, obtainable from Microsoft
or bundled with your C++ compiler.

Generating wxHTML Help format
=============================

To generate wxHTML Help documentation, you need to generate
the HTML files as above and then archive the HTML, image,
hhc, hhp and optionally hhk files into fl.htb.

The helpview program can be used to view fl.htb
on supported platforms. Get it from the wxWindows web site
or compile it with samples/html/helpview in the wxWindows
distribution.

Generating WinHelp format
=========================

To generate Windows Help documentation, you need to generate
the RTF file and contents file (.cnt) using --winhelp and
then run hc.exe to produce the .hlp file 

Generating PDF format
=====================

Run Tex2RTF with the -rtf argument, load the RTF into MS Word,
select the whole document, press F9, then apply wordstyle.dot
found in distrib/msw in the wxWindows distribution.
Then generate the PDF using Adobe Acrobat PDFWriter.

Note that for the contents to be formatted correctly,
each class section should end with a blank line. Otherwise
the contents will have bits of the reference in it.

Generating .tex files from .h files
===================================

Much of the class reference is generated from the headers.
The intention is to comment the headers more thoroughly, then
regenerate the .tex files, before working directly on the
.tex files.

To regenerate .tex files, you'll need the HelpGen utility from
latest wxWindows CVS, in utils/helpgen/src. Compile wxBase,
then HelpGen. Run helpgen with e.g.

  helpgen dump file.h

and a separate .tex file for each class will appear in the
current directory.

--
Julian Smart, January 3rd 2002
julian.smart@btopenworld.com

