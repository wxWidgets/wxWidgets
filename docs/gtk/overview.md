# wxWidgets on the GNOME Desktop       {#plat_gtk_overview}

wxWidgets is a C++ cross-platform GUI library, whose distintive feature is the
use of native calls and native widgets on the respective platform, i.e. an
application compiled for the Linux platform will use the [GTK+][] library for
displaying the various widgets. There is also a version ("port") of wxWidgets
which uses the Motif toolkit for displaying its widgets (this port is commonly
referred to as wxMotif) and another one, which only uses X11 calls and which
draws its widgets entirely itself, without using any outside library. This port
is called wxX11 or sometimes more generally wxUniv (short for wxUniversal),
since this widget set (implemented entirely within wxWidgets) is available
wherever wxWidgets is available. Since this short overview is mainly about how
to write wxWidgets applications for the [GNOME][] desktop, I will focus on the
GTK+ port, which is generally referred to as wxGTK.

wxGTK still supports the old version GTK+ 1.2, but it now defaults to the
up-to-date version GTK+ 2.X, which is the basis for the current GNOME desktop. By
way of using GTK+ 2.X and its underlying text rendering library [Pango][], wxGTK
fully supports the Unicode character set and it can render text in any language
and script, that is supported by Pango.

[GTK+]: http://www.gtk.org/
[GNOME]: https://www.gnome.org/
[Pango]: http://www.pango.org/

## Design Principles

The three main design goals of the wxWidgets library are portability across the
supported platforms, complete integration with the supported platforms and a
broad range of functionality covering most aspects of GUI and non-GUI
application programming. Sometimes, various aspects of these design goals
contradict each other and this holds true especially for the Linux platform
which – from the point of view of the desktop environment integration – is
lagging behind the other two major desktops (Windows and OS X) mostly because of
the schism between the GTK+ based GNOME desktop and the [Qt][] based [KDE][]
desktop. So far, the typical wxWidgets user targeted Windows, maybe OS X and
Linux _in general_, so the aim was to make wxGTK applications run as well as
possible on as many versions of Linux as possible, including those using the KDE
environment. Luckily, most of these distributions included the GTK+ library (for
running applications like the GIMP, GAIM, Evolution or Mozilla) whereas the
GNOME libraries were not always installed by default. Also, the GNOME libraries
didn't really offer substantial value so that the hassle of installing them was
hardly justified. Therefore, much effort was spent on making wxGTK fully
functional without relying on the GNOME libraries, mostly by reimplementing as
much as sensible of the missing functionality. This included a usable file
selection dialog, a printing system for PostScript output, code for querying
MIME-types and file-icon associations, classes for storing application
preferences and configurations, the possibility to display mini-apps in the
taskbar, a full-featured HTML based help system etc. With all that in place you
can write a pretty fully featured wxWidgets application on an old Linux system
with little more installed than X11 and GTK+.

[Qt]: http://www.qt.io/
[KDE]: https://www.kde.org/

## Recent Developments

Recently, several key issues have been addressed by the GNOME project. Sometimes
integrated into the newest GTK+ releases (such as the file selecter), sometimes
as part of the GNOME libraries (such as the new printing system with Pango
integration or the mime-types handling in gnome-vfs), sometimes as outside
projects (such as the media/video backend based on the [Gstreamer][] project).
Also, care has been taken to unify the look and feel of GNOME applications by
writing down a number of rules (modestly called
["Human Interface Guidelines"][GNOME-HIG]) and more and more decisions are taken
in a desktop neutral way (for both GNOME and KDE), mostly as part of the
[FreeDesktop][] initiative. This development together with the rising number of
OpenSource projects using wxWidgets mainly for the Linux and more specifically
GNOME desktop has led to a change of direction within the wxWidgets project, now
working on making more use of GNOME features when present. The general idea is
to call the various GNOME libraries if they are present and to offer a
reasonable fallback if not. I'll detail on the various methods chosen below:

[Gstreamer]: http://gstreamer.freedesktop.org/
[GNOME-HIG]: http://developer.gnome.org/projects/gup/hig
[FreeDesktop]: http://www.freedesktop.org/

## Printing System

The old printing system ....

## MIME-type Handling

The old mime-type system used to simply query some files stored in "typical"
locations for the respective desktop environment. Since both the format and the
location of these files changed rather frequently, this system was never fully
working as desired for reading the MIME-types and it never worked at all for
writing MIME-types or icon/file associations. ...

## File Dialogs

Previously, wxGTK application made use of a file dialog written in wxWidgets
itself, since the default GTK+ file dialog was simplistic to say the least. This
has changed with version GTK+ 2.4, where a nice and powerful dialog has been
added. wxGTK is using it now.

## File Configuration and Preferences

The usual Unix way of saving file configuration and preferences is to write and
read a so called "dot-file", basically a text file in a user's home directory
starting with a dot. This was deemed insufficient by the GNOME desktop project
and therefore they introduced the so called GConf system, for storing and
retrieving application and sessions information....

## Results and Discussion

One of wxWidgets' greatest merits is the ability to write an application that
not only runs on different operating systems but especially under Linux even on
rather old systems with only a minimal set of libraries installed – using a
single application binary. This was possible since most of the relevant
functionality was either located in the only required library (GTK+) or was
implemented within wxWidgets. Recent development outside the actual GTK+ project
has made it necessary to rethink this design and make use of other projects'
features in order to stay up-to-date with current techological trends. Therefore,
a system was implemented within wxWidgets that queries the system at runtime
about various libraries and makes use of their features whenever possible, but
falls back to a reasonable solution if not. The result is that you can create
and distribute application binaries that run on old Linux systems and integrate
fully with modern desktops, if they are available. This is not currently
possible with any other software.

Copyright 2004 © Robert Roebling, MD. No reprint permitted without written prior
authorisation. Last modified 14/11/04.

## About the Author

Robert Roebling works as a medical doctor in the Department of Neurology at the
University clinic of Ulm in Germany. He has studied Computer Sciences for a few
semesters and is involved in the wxWidgets projects since about 1996. He has
started and written most of wxGTK port (beginning with GTK+ around 0.9) and has
contributed to quite a number projects within wxWidgets, ranging from the image
classes to Unicode support to making both the Windows and the GTK+ ports work on
embedded platform (mostly PDAs). He is happily married, has two children and
never has time.
