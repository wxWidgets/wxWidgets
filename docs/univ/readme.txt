			      wxUniversal README
			      ==================

 Welcome to wxUniversal!

 Acknowledgements: wxUniversal wouldn't have been written without the
generuous support of SciTech Software. Many thanks to Kendall Benett and
Tom Ryan!

0. Introduction
---------------

 wxUniversal is a port of wxWindows which implements the various GUI controls
by drawing them itself (using low level wxWindows classes). Please see

		     http://www.wxwindows.org/wxuniv.htm

for more details about it.

 The advantage of wxUniversal is that you have precise control over the
controls appearance (it is not always possible to change all aspects of the
native controls) and the theme support: the same program may be changed to
look completely differently  without changing a single line of its code but
just changing the theme.

 Another advantage is that it makes writing ports of wxWindows for other
platforms (such as OS/2, BeOS or QNX) much simpler, so it is of special
interest to the people interested in porting wxWindows to another platform.

 However, wxUniversal doesn't have the 100% native look and feel unlike the
other wxWindows ports - this is the price to pay for the extra flexibility.

1. Requirments and supported platforms
--------------------------------------

 wxUniversal is used together with another wxWindows port which provides the
"low level classes" mentioned above. Currently it can be built either with
wxMSW or wxGTK. In any case, you should download the sources for the
appropriate toolkit in addition to wxUniversal - in fact, you should download
them first and then unpack wxUniversal in the same directory.

 The requirments for wxUniversal are the same as for the underlying low level
toolkit.

2. Installing under Win32
-------------------------

a) Using Visual C++ 6.0

   Simply open the src/wxUniv.dsw file in MSDEV and build it.

b) Cygwin

   Please refer to the Unix section below

c) Other compilers

   Unfortunately we don't have the makefiles for any other compilers yet.
Please contact us if you would like to help us with creating one for the
compiler you use.

3. Installing under Unix
------------------------

Please refer to the installation instructions for wxGTK. The only change for
wxUniversal is that you must add "--enable-univ" switch to the configure
command line.

4. Documentation and support
----------------------------

 Please note that wxUniversal is not as mature as the other wxWindows ports
and is currently officially in alpha stage. In particular, it is not really
intended for the end users but rather for developers at the current stage and
this is why we don't provide any binaries for it.

 There is no separate documentation for wxUniversal, please refer to wxWindows
documentation instead.

 Support for wxUniversal is available from the same places as for wxWindows
itself, namely:

* Usenet newsgroup comp.soft-sys.wxwindows

* Mailing lists: see http://lists.wxwindows.org/ for more information

* WWW page: http://www.wxwindows.org/


 Hope you will find wxUniversal useful!

								 Vadim Zeitlin


This file is accurate for the version 2.3.2 of wxUniversal. It was last
modified on Dec 11, 2001.


