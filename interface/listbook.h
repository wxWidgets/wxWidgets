/////////////////////////////////////////////////////////////////////////////
// Name:        listbook.h
// Purpose:     interface of wxListbook
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxListbook
    @wxheader{listbook.h}

    wxListbook is a class similar to wxNotebook but which
    uses a wxListCtrl to show the labels instead of the
    tabs.

    There is no documentation for this class yet but its usage is
    identical to wxNotebook (except for the features clearly related to tabs
    only), so please refer to that class documentation for now. You can also
    use the @ref overview_samplenotebook "notebook sample" to see wxListbook in
    action.

    @beginStyleTable
    @style{wxLB_DEFAULT}:
           Choose the default location for the labels depending on the current
           platform (left everywhere except Mac where it is top).
    @style{wxLB_TOP}:
           Place labels above the page area.
    @style{wxLB_LEFT}:
           Place labels on the left side.
    @style{wxLB_RIGHT}:
           Place labels on the right side.
    @style{wxLB_BOTTOM}:
           Place labels below the page area.
    @endStyleTable

    @library{wxcore}
    @category{miscwnd}

    @see wxBookCtrl(), wxNotebook, @ref overview_samplenotebook "notebook sample"
*/
class wxListbook : public wxBookCtrl overview
{
public:
    //@{
    /**
        Constructs a listbook control.
    */
    wxListbook();
    wxListbook(wxWindow* parent, wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxEmptyStr);
    //@}
};

