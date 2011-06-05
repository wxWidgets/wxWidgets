/////////////////////////////////////////////////////////////////////////////
// Name:        using.h
// Purpose:     Usage page for the wxMobile Doxygen manual
// Author:      Julian Smart
// RCS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


/**

@page page_using Using wxMobile

@li @ref page_using_overview
@li @ref page_using_app
@li @ref page_using_frames
@li @ref page_using_panels
@li @ref page_using_basic_controls
@li @ref page_using_tables
@li @ref page_using_sheets
@li @ref page_using_toolbars
@li @ref page_using_notebooks
@li @ref page_using_navcontrols
@li @ref page_using_wheelsctrl
@li @ref page_using_images
@li @ref page_using_fonts

<hr>


@section page_using_overview Overview

wxMobile is provided as a set of source files that you can compile and link with
your application, together with a recent version of desktop wxWidgets. Currently
there is no real build system for wxMobile but you can use DialogBlocks to generate
makefiles and project files for a variety of compilers, or you can add the
sources to a project using the IDE of your choice.

wxMobile consists of two main elements:


@li a set of controls;
@li a simulator user interface.

The controls look like and behave in a similar way to the Cocoa Touch
controls, with a wxWidgets-like API that mimics the wxiPhone port as far as possible.

The simulator user interface displays an application in a window of the
appropriate resolution, with commands for changing the aspect ratio
('rotating' the device) and other means of simulating device behaviour.

While the simulated controls are not identical in appearance and behaviour
to the real thing, they are similar enough to allow development and (to
some extent) testing of the application.

To compile for the real device or Apple SDK simulator, the developer
simply switches project configuration and builds against the wxiPhone
port.


To allow this to happen, the developer should use classes prefixed
with wxMo, such as wxMoApp, wxMoToolBar and so on, so they can
be mapped to both the wxiPhone and wxMobile libraries as necessary.
Include files such as wx/mobile/app.h - when compiling for wxiPhone,
the appropriate wxWidgets include files will be used in place
of the wxMobile code.

The following topics (and the wxMobile reference) can be used as a
guide to the wxiPhone API, as well as the wxMobile API, since they
are largely the same.

Note: at the time of writing, there is no significant wxiPhone
API, since the port is at a preliminary stage. However, it is intended
that the two libraries converge to a common API although they
are being written at different times by different developers.

@section page_using_app The application class

Derive your application class from wxMoApp. Call wxMoApp::OnInit at the
beginning of your OnInit function, and wxMoApp::OnExit at the end of your
OnExit function. Otherwise, the simulator will not be set up and cleaned up properly.
It's good practice to call the base functions anyway.

From within your OnInit function, create a wxMoFrame for your top-level frame.
Currently wxMobile supports only one wxMoFrame per application. Avoid the use
of wxApp::SetTopWindow and wxApp::GetTopWindow, since for a one-frame application
it's unnecessary, and on the simulator the top window is the simulator frame
which will not usually be what the application expects.

Your wxMoApp::OnInit function might look something like this, for a simple application:

@code
bool MobileDemoApp::OnInit()
{
    wxMoApp::OnInit();

    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxJPEGHandler);
    wxImage::AddHandler(new wxGIFHandler);

    DemoFrame* mainWindow = new DemoFrame(NULL);
    mainWindow->Show(true);

    return true;
}
@endcode

@section page_using_frames Frames and dialogs in wxMobile

As mentioned in the previous section, only one frame is currently permitted at a time.
Since applications must be simplified on a portable device, a single frame makes a lot
of sense. We'll see controls in later sections that make it easy to show different windows
within the same frame.

When compiling for wxMobile, wxMoFrame isn't really implemented as a top-level frame,
since it must have the simulator window as its parent. So you need to avoid
making the assumption that wxMoFrame is derived from wxTopLevelWindow. The only
wxFrame-like API that wxMoFrame has is SetTitle/GetTitle (currently unused), and SendSizeEvent.
It also behaves like a wxFrame with respect to child windows - it fits a single
child inside the frame, or uses sizer layout if a top-level sizer is present.

wxMoFrame has no caption, menubar, built-in toolbar, or status bar, and most wxFrame styles will
be ignored. The frame will always be sized to the whole device screen size, minus the system
status bar.

Custom dialogs and modal convenience dialogs are not implemented in wxMobile, because
the Touch API doesn't have an equivalent, nor does it support modal event loops.
Visually, the closest thing to a dialog is a sheet, which we will discuss later.
But for the most part, elements you might normally design as dialogs will be
implemented using navigation controls and tables.

@section page_using_panels Panels and scrolled windows

wxMoPanel and wxMoScrolledWindow should be used in place of the usual wxPanel and wxScrolledWindow
classes. When using wxMoScrolledWindow, specify one pixel per unit. You can also
use wxMoVScrolledWindow to implement vertically scrolling windows - see the
documentation for wxVScrolledWindow in the wxWidgets manual for how to use the
class.

On Cocoa Touch, scrolling windows do not have any scrollbars, just some optional
indicators that appear during scrolling. wxMobile follows Cocoa Touch in redirecting
touch events to the underlying window if the gesture turns out not to be a scrolling action.
wxMobile also sends a wxEVT_TOUCH_SCROLL_CANCEL_TOUCHES to cancel the current content window mouse handling
if the touch turns out to be a drag after all. You can see this in operation in the
table control. If you drag quickly, no selection is activated and it scrolls as expected.
If you click and hold, the row will be selected. If you were to release without moving,
the expected row action will occur, but if you hold and move, the selection is cleared and scrolling begins.

Currently mouse event handling can only be cancelled for windows that are derived from
wxMoScrolledWindow, and not for child controls inside the scrolled window.
For this reason, scrolling when there are child windows will not generally work quite as effectively
as on the real device. However, for custom child windows, you can help the simulator by calling
wxScrolledWindow::ForwardMouseEvents in your custom window mouse event handler, at least if your
custom window is for display purposes only. If you need to handle the mouse events in your custom window that's
a child of a scrolling window such as a table, then forwarding the events to the scrolled window isn't going to
be possible since you need the events for your custom control behaviour (and wxMobile isn't clever enough
to know which child control to send a cancel touch event to). But on the simulator you can still test
scrolling in the application by touching areas of the scrolled window not covered by child windows.

Note that multi-touch and zooming hasn't been implemented by wxMobile yet.

@section page_using_basic_controls Basic controls

The basic controls are:

@li wxMoButton: a text button.
@li wxMoBitmapButton: a bitmap button.
@li wxMoStaticText: a static text label.
@li wxMoStaticBitmap: a static bitmap label.
@li wxMoTextCtrl: a single and multiline text control.
@li wxMoSearchCtrl: a search text control.
@li wxMoSwitchCtrl: a boolean control.
@li wxMoSlider: a slider control.
@li wxMoGauge: a gauge control.
@li wxMoListBox: a listbox implemented in terms of a wxMoWheelsCtrl. This UI is under review.
@li wxMoWebCtrl: displays local or remote HTML pages. This is implemented using wxHtmlWindow
on some platforms, so don't expect rendering to always be as good as the real control.

Currently there are no equivalents for wxComboBox and wxChoice, although these should probably
be implemented with a text control and a button that shows a wxMoWheelsCtrl.

wxMoCheckBox is also missing, since wxMoSwitchCtrl replaces it for boolean values. However,
it could perhaps be implemented using a bitmap button and a static text control.

Other controls that are NOT available include wxAnimationCtrl, wxBitmapComboBox,
wxToggleButton, wxCalendarCtrl, wxCheckListBox, wxComboCtrl, wxDataViewCtrl,
wxGenericDirCtrl, wxHtmlListBox, wxSimpleHtmlListBox, wxListCtrl, wxRichTextCtrl,
wxTreeCtrl, wxScrollBar, wxSpinButton, wxSpinCtrl, wxHyperlinkCtrl, wxRadioBox,
wxRadioButton, and the various picker controls.

Some of these may be implemented over time but almost certainly not the more complex ones
such as wxListCtrl, wxTreeCtrl or wxRichTextCtrl.

@section page_using_tables Tables

The table is used in most Touch apps because it's great at laying out compact lists of information with arbitrary length,
for example application settings, a list of media, or an index to various parts of the program. Tables are
scrollable and although they consist of only one column, that column can contain an icon on the left, a main text label,
optional detail text, and (optionally) buttons or other controls.

Tables come in two main styles: grouped, or plain. Although both styles support multiple sections, the grouped style
emphasises section grouping with a section header and rounded section border. The plain style supports an optional
index on the right of the table, each label of which scrolls to the appropriate section when touched.

Each row has an associated cell (wxMoTableCell) which can itself take a variety of styles, for example displaying
a main text label with a detail text label underneath it as used by the audio player application.

If the existing cell styles are not sufficient, the application can either place controls on the content
window associated with the cell, or assign a window to the accessory area, for example a wxMoSwitchCtrl
as the demo shows.

A row can have a selected state but Apple guidelines state that you should immediately deselect the row
when changing the view as a result of the selection.

wxMobile currently supports editing mode for insertion, deletion and row reordering. Row reordering is visually
different from Cocoa Touch, showing an insertion marker to the right of the row instead of graphically previewing the
new row order. However this is sufficient to test appropriate application behaviour.

@subsection page_using_tables_data_source Providing data to a table

To populate a wxMoTableCtrl, you don't have to derive a new class from it but you do need
to give it a data source. Derive a new class from wxMoTableDataSource
and implement various functions that let the table know about data and
cell characteristics. Here are descriptions of the important functions.

wxMoTableDataSource::GetCell returns the cell object for a given wxTablePath.
You should first call wxMoTableCtrl::GetReusableCell with a unique name for this
cell type to see if the table can use an existing cell that is not currently displayed.
If one is available, set its properties and return that cell; otherwise, create a
a new wxMoTableCell.

Implement wxMoTableDataSource::GetSectionCount to return the number of sections
in the table. This is relevant even for the plain style, since there can still
be section names.

wxMoTableDataSource::GetRowCount should return the number of rows in a given section.

If wxMoTableDataSource::GetRowHeight returns a number greater than zero, the value is
used for the height of the given row.

To supply section titles, return an array of strings from wxMoTableDataSource::GetSectionTitles.

If you're using the wxTC_PLAIN style you can return an array of strings for the index
on the right hand side using wxMoTableDataSource::GetIndexTitles, with the same number of strings are there are sections.
This will create an index that automatically takes the display to the start of the
relevant section when index items are clicked.


@section page_using_sheets Sheets

Sheets are a bit like dialogs, but are restricted in the information they can present,
and they cannot be modal.

wxMoActionSheet is anchored from an edge of the screen and
can take a title, OK (destructive action) button, Cancel button,
and further custom buttons.

wxMoAlertSheet is a free-floating window and can take a
title, Cancel button, and further custom buttons.

Events of type wxSheetEvent are sent to the sheet (or owner, if SetOwner
has been called); if these fail, ordinary button command events are sent.

The standard wxID_OK and wxID_CANCEL identifiers are used, and for further
custom butons, the event identifiers can be retrieved using wxMoSheetBase::GetButtonId.

Since sheets cannot be modal, you will need to organise your event handling
differently from normal -  you cannot block and wait for the sheet to be closed.
There is a wxIBMessageBox function with a similar API to wxMessageBox,
but it always returns immediately so you should ignore the return value.

@section page_using_toolbars Toolbars

See: wxMoToolBar

This section to be completed soon.

@section page_using_notebooks Tab controls, notebooks and segmented controls

See: wxMoTabCtrl, wxMoNotebook, wxMoSegmentedCtrl

This section to be completed soon.

@section page_using_navcontrols Navigation bars and navigation controls

wxMoNavigationBar and wxMoNavigationCtrl help you create a user
interface with a web-like structure, where you push a view onto a stack
and allow the user to navigate back with a Back button.

wxMoNavigationBar manages a stack of wxMoNavigationItem objects.
It shows a centred title, and a Back button on the left of the bar if
there is one or more item under the current item.

The bar can also be customised by adding a user-defined wxMoBarButton to the
left or right of the bar.

wxMoNavigationCtrl uses wxMoNavigationBar, but manages a stack of wxMoViewController
objects. Each wxMoViewController has a title and a window to be shown when
this view controller is at the top of the stack. When the user clicks on Back,
the view controller (and its window) is deleted and the previous window
is shown. The developer can override the default behaviour of deleting the
controller and/or the window automatically, for example if the windows are
time-consuming to create and should be cached.

When combined with wxMoTableCtrl in particular, powerful but intuitive user interfaces can be created.

@section page_using_wheelsctrl Wheel controls

See: wxMoWheelsCtrl

This section to be completed soon.

@section page_using_images Working with images

UI elements such as toolbars and tab controls requires image files (normally PNG) with alpha-channel transparency.
To avoid having to load these images at run-time, you can embed them in your C++
source as you might with XPM, though the procedure is a little different.
Use DialogBlocks or bin2c (see http://wiki.wxwidgets.org/Embedding_PNG_Images) to generate
the C++ data, perhaps with a .inc sufix, and then use code like this to
load the image:

@code
// Get a bitmap, such as a PNG, from raw data
wxBitmap GetBitmapFromData(const unsigned char *data, int length)
{
    wxMemoryInputStream is(data, length);
    return wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);
}

#include "toolimage_24x24.inc"

{
    wxBitmap bitmap = GetBitmapFromData(toolimage_24x24, sizeof(toolimage_24x24));
}

To create the images in the first place, you can use a paint program such as PaintShop Pro.
Typically, you create an image on a transparent background, create a new mask from the image using
its opacity, and then save the mask to an alpha channel.
@endcode

@section page_using_fonts Working with fonts

wxMobile does not yet handle the disparity between font sizes on the various desktop systems compared
with font sizes on Touch platforms. However, in most cases, the default fonts will be the most
appropriate, and will therefore not need explicit setting.

*/
