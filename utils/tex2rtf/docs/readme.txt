
+++++                    Announcing Tex2RTF                      +++++
+++++  A Free LaTeX to RTF, Windows Help RTF, and HTML converter +++++

Purpose
=======

  *  Allows you to maintain manuals in printed and hypertext formats.

  *  Allows conversion of existing LaTeX documents to word processor
     formats (usually some 'tweaking' is required to conform to
     Tex2RTF restrictions).

Input
=====

A LaTeX subset with some additional hypertext macros; no maths,
minimal table support.

Caveat: Please do not expect to convert arbitrary LaTex files without
editing: this is not the primary intention of Tex2RTF. Success converting
existing LaTeX depends on your expectations and the style in which
the LaTeX was written. Tex2RTF does not accept pure TeX (non-LaTeX)
documents.

Output
======

  *  ordinary RTF

  *  Windows Help hypertext RTF

  *  HTML (the World Wide Web hypertext format)

  *  wxHTML Help (the wxWidgets GUI library help file format)

Installation
============

Under Windows, please run the supplied install.exe program. The Tex2RTF
manual itself serves as an example input file, in the docs subdirectory
with various .sty, .ini, and .gif files that you may need for your
own documents. See the online manual for further details.

Platforms supported
===================

  *  Windows (all 32-bit desktop versions)

  *  Any Unix supporting wxWidgets

  *  Mac OS X

Where to get it
===============

The latest version of Tex2RTF can be accessed by anonymous ftp
from:

    ftp://biolpc22.york.ac.uk/pub/tex2rtf

The WWW pages are at:

    http://www.wxwidgets.org/tex2rtf

Tex2RTF was developed using the free, cross-platform GUI toolkit
wxWidgets, at http://www.wxwidgets.org. To compile Tex2RTF
for platforms for which there are no Tex2RTF binaries,
you need to download an appropriate version of wxWidgets,
configure and compile the library, then compile Tex2RTF
in utils/tex2rtf/src.

------------------------------------------------------------------
Julian Smart, August 2002
julian.smart@btopenworld.com



