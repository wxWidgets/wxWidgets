/////////////////////////////////////////////////////////////////////////////
// Name:        choicebk.h
// Purpose:     interface of wxChoicebook
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxChoicebook
    @wxheader{choicebk.h}

    wxChoicebook is a class similar to wxNotebook but which
    uses a wxChoice to show the labels instead of the
    tabs.

    There is no documentation for this class yet but its usage is
    identical to wxNotebook (except for the features clearly related to tabs
    only), so please refer to that class documentation for now. You can also
    use the @ref overview_samplenotebook "notebook sample" to see wxChoicebook in
    action.

    wxChoicebook allows the use of wxBookCtrl::GetControlSizer, allowing a program
    to add other controls next to the choice control. This is particularly useful
    when screen space is restricted, as it often is when wxChoicebook is being
    employed.

    @beginStyleTable
    @style{wxCHB_DEFAULT}:
           Choose the default location for the labels depending on the current
           platform (left everywhere except Mac where it is top).
    @style{wxCHB_TOP}:
           Place labels above the page area.
    @style{wxCHB_LEFT}:
           Place labels on the left side.
    @style{wxCHB_RIGHT}:
           Place labels on the right side.
    @style{wxCHB_BOTTOM}:
           Place labels below the page area.
    @endStyleTable

    @library{wxcore}
    @category{miscwnd}

    @see wxBookCtrl(), wxNotebook, @ref overview_samplenotebook "notebook sample"
*/
class wxChoicebook : public wxBookCtrl overview
{
public:
    //@{
    /**
        Constructs a choicebook control.
    */
    wxChoicebook();
    wxChoicebook(wxWindow* parent, wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxEmptyStr);
    //@}
};

