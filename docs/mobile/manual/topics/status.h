/////////////////////////////////////////////////////////////////////////////
// Name:        status.h
// Purpose:     Status page for the wxMobile Doxygen manual
// Author:      Julian Smart
// RCS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

/**

@page page_status Project status

The generic wxMobile implementation currently supports most of the Cocoa Touch controls, although often with
some restrictions.

Until the wxiPhone port is more advanced, wxMobile only works in simulation. Besides supporting wxiPhone,
major goals include:

@li Configuration via command-line tools.
@li Decide whether some of the generic controls should go into regular wxWidgets.
@li Add some extra functionality into core wxWidgets, such as support for touch screens in wxMouseEvent.
@li Further discussion and definition of the wxMobile API.
@li A wxWinCE port, mixing native and generic controls.
@li Investigation of how Maemo might fit into wxMobile.
@li API for sound and 3D graphics.
@li A full guide to writing applications with wxMobile.

Here are some of the detailed items left to do in the generic implementation:

@li Implement wxMoButton styles e.g. wxBU_DISCLOSURE, wxBU_INFO_LIGHT, wxBU_INFO_DARK, wxBU_CONTACT_ADD.
@li Alignment modes and indentation for table cells.
@li Table headers and footers.
@li Consider providing standard table cell 'widgets' e.g. wxTableCellChoice which spawns another controller,
assuming the table is part of a navigation control. See the Edit Phone screen of the Contacts app,
where the type of number (e.g. home, work etc.) can be chosen from a list that's shown when you click
on the row. On this list, there can be a control to enter a value as per wxComboCtrl.
@li System-defined constants for dimensions such as row height; see e.g. kUIRowHeight.
@li Implement non-iPhone visual styles, such as for the table control under WinCE.
@li Validators.
@li On-screen keyboard, showing when the user clicks on text.
@li Support simulated bundles in some way.
@li First responder functions.
@li Wheel/selector control needs to be showable at bottom - how is this normally done?
@li Multitouch events and simulation.
@li Application low memory and close events.
@li Startup protocol e.g. showing blank screen image as per guidelines.
@li Circular progress control.
@li Support zooming in scrolled window.
@li Simulator warnings about guideline infringements.
@li Consider emulating all major wxWidgets controls e.g. wxComboCtrl by using
  composite controls, the way it's done in Safari - pop up a detailed editor.


*/
