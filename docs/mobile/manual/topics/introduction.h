/////////////////////////////////////////////////////////////////////////////
// Name:        introduction.h
// Purpose:     Introduction page for the wxMobile Doxygen manual
// Author:      Julian Smart
// RCS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


/**

@page page_introduction Introduction

@li @ref page_introduction_whatis
@li @ref page_introduction_requirements
@li @ref page_introduction_acknowledgements


<hr>


@section page_introduction_whatis What is wxMobile?

Since mobile devices present different controls, challenges and restrictions compared with desktop
computers, one wxWidgets API cannot fit all needs and a different approach needs to be taken.
wxMobile is an API that is intended to help programmers provide mobile GUIs on several mobile platforms,
while retaining as much of the wxWidgets API as possible. So in some ways wxMobile is a subset of wxWidgets,
but it also introduces new features that are particularly relevant to mobile devices.

wxMobile also provides a way of developing mobile applications on desktop computers via simulation.
Because of this, existing wxWidgets classes have different names on wxMobile - for example wxFrame
becomes wxMoFrame. This allows the classes to map to simulated or native classes depending
on the required build.

The starting point for wxMobile is the Cocoa Touch (iPhone/iPod) API. Many of the concepts used
in this API are applicable to other mobiles, even if they are not implemented natively in that
device's API. So in order to achieve a unified wxMobile API, we lean towards the Cocoa Touch
API and provide similar controls on other platforms. The author takes the position that it's
not possible to use a more generic API that maps to different controls on different
devices - instead we have to make concrete decisions about the core set of controls for our applications,
and the iPhone UI leads the way. Also, since Apple have strict requirements for inclusion
in their online store, wxMobile must allow detailed conformance or it risks encouraging the creation
of unpublishable applications. Since other platforms are not so strictly policed, the UI can deviate
from the native look and feel without such dire consequences. Of course, we can aim to provide
native UIs for specific devices with little extra programmer effort.

At present, there is only one way of building wxMobile - as a simulation on a desktop computer,
using the Windows, GTK+ or Mac port. When the wxWidgets Cocoa Touch port matures, an
iPhone wxMobile port will be possible. The next target will be to adapt wxMobile to
Windows Mobile (wxWinCE) using a mixture of generic and native controls.

Even if wxMobile only targets a couple of mobile platforms - wxiPhone and perhaps wxWinCE - wxMobile's
API and documentation will be helpful in guiding the programmer through the specific requirements of
mobile development. It will also allow the programmer to use existing RAD tools (especially if they are
adapted for wxMobile) and to do much development and debugging on familiar environments
instead of being constrained by the vendor's toolset and development platform requirements.
Without the wxMobile API, the wxWidgets programmer could still use the wxiPhone port but
without being able to compile the application on (say) a Windows machine and without
specific RAD tool support for wxiPhone controls.

Please note that there is no substitute for testing and deployment using a specific port such as wxiPhone
and the native vendor tools. However, wxMobile reduces dependence on those tools for much
of the application design and development work, as well as making it easier to learn
(for example) wxiPhone programming principles without access to a Mac or a physical iPhone.

@subsection page_introduction_why Why not just use wxWidgets directly?

You may still be a bit baffled about what wxMobile is, compared with the wxiPhone or wxWinCE port. Why
not just use those ports directly?

You can indeed use them directly, and these ports are independent of wxMobile. However, you wouldn't then
get the benefit of simulation on a desktop machine (aside from Apple's own emulator), and neither
would you be able to use the special mobile-friendly controls such as wxTableCtrl or wxNavigationCtrl
on platforms other than iPhone. Now these generic controls may well make it into core wxWidgets eventually,
but there will still be help from wxMobile to smooth out the differences between mobile platforms.


@section page_introduction_requirements wxMobile requirements

To make use of wxMobile, you currently need one of the following setups.

(a) Microsoft Windows:

@li A 32-bit or 64-bit PC running MS Windows.
@li A Windows compiler, preferably MS Visual C++, but you may have luck with Borland C++, Watcom C++, Cygwin, or MinGW.

(b) Unix:

@li Almost any C++ compiler, including GNU C++ and many Unix vendors
    compilers such as Sun CC, HP-UX aCC or SGI mipsPro.
@li Almost any Unix workstation, and one of: GTK+ 2.4 or higher.

(c) Mac OS/Mac OS X:

@li A PowerPC or Intel Mac running Mac OS X 10.4 or higher
@li The Apple Developer Tools (eg. GNU C++)

Under all platforms it's recommended to have large amounts of free hard disk
space.


@section page_introduction_where Availability and location of wxMobile

wxMobile is currently available from http://www.anthemion.co.uk/mobile.


@section page_introduction_acknowledgements Acknowledgements

wxMobile comes to you courtesy of the following:

@li Julian Smart
@li Stefan Csomor
@li The wxWidgets team (for wxWidgets)


*/
