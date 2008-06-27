/////////////////////////////////////////////////////////////////////////////
// Name:        toolbook.h
// Purpose:     interface of wxToolbook
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxToolbook

    wxToolbook is a class similar to wxNotebook but which uses a wxToolBar to
    show the labels instead of the tabs.

    There is no documentation for this class yet but its usage is identical to
    wxNotebook (except for the features clearly related to tabs only), so please
    refer to that class documentation for now. You can also use the
    @ref page_samples_notebook to see wxToolbook in action.

    @beginStyleTable
    @style{wxTBK_BUTTONBAR}
        Use wxButtonToolBar-based implementation under Mac OS (ignored under
        other platforms).
    @style{wxTBK_HORZ_LAYOUT}
        Shows the text and the icons alongside, not vertically stacked (only
        implement under Windows and GTK 2 platforms as it relies on @c
        wxTB_HORZ_LAYOUT flag support).
    @endStyleTable

    The common wxBookCtrl styles described in the @ref overview_bookctrl are
    also supported.

    @library{wxcore}
    @category{miscwnd}

    @see @ref overview_bookctrl, wxBookCtrlBase, wxNotebook,
         @ref page_samples_notebook
*/
class wxToolbook : public wxBookCtrlBase
{
public:

};

