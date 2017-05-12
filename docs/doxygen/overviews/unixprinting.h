/////////////////////////////////////////////////////////////////////////////
// Name:        unixprinting.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_unixprinting Printing Under Unix (GTK+)

Printing under Unix has always been a cause of problems as Unix does not
provide a standard way to display text and graphics on screen and print it to a
printer using the same application programming interface - instead, displaying
on screen is done via the X11 library while printing has to be done with using
PostScript commands. This was particularly difficult to handle for the case of
fonts with the result that only a selected number of application could offer
WYSIWYG under Unix. Equally, wxWidgets offered its own printing implementation
using PostScript which never really matched the screen display.

Since GTK+ 2.10, support for printing has been added to GTK+ itself and
beginning with wxWidgets 2.9, GTK+ printing is used by default (i.e. unless
<tt>\--without-gtkprint</tt> was explicitly used when configuring the library).
Support for GTK+ print is detected dynamically, i.e. during the run-time: if it
is found, printing will be done through GTK+, otherwise the application will
fall back to the old PostScript printing code. This allows the applications
built with wxWidgets to still work on the very old systems using GTK+ earlier
than 2.10.

*/
