/////////////////////////////////////////////////////////////////////////////
// Name:        _srchctrl.i
// Purpose:     SWIG interface for wxSearchCtrl
//
// Author:      Robin Dunn
//
// Created:     9-Dec-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module

//---------------------------------------------------------------------------
%newgroup

MAKE_CONST_WXSTRING(SearchCtrlNameStr);


%{
#if !defined(wxUSE_SEARCHCTRL) || !wxUSE_SEARCHCTRL
// define a dummy class for builds that don't have wxSearchCtrl

enum {
    wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN,
    wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN
};

class wxSearchCtrl : public wxTextCtrl
{
public:
    wxSearchCtrl() { wxPyRaiseNotImplemented(); }
    wxSearchCtrl(wxWindow*, wxWindowID,
                 const wxString&,
                 const wxPoint&,
                 const wxSize&,
                 long style,
                 const wxValidator&,
                 const wxString& name)
    { wxPyRaiseNotImplemented(); }

    bool Create( wxWindow*, wxWindowID,
                 const wxString&,
                 const wxPoint&,
                 const wxSize&,
                 long style,
                 const wxValidator&,
                 const wxString& name) {}
        
    virtual void SetMenu( wxMenu* ) {}
    virtual wxMenu* GetMenu() { return NULL; }
    
    // get/set search options
    // ----------------------
    virtual void ShowSearchButton( bool ) {}
    virtual bool IsSearchButtonVisible() const { return false; }

    virtual void ShowCancelButton( bool ) {}
    virtual bool IsCancelButtonVisible() const { return false; }
};    
#endif
%}



// Now define the class for SWIG

MustHaveApp(wxSearchCtrl);
DocStr(wxSearchCtrl,
"A search control is a composite of a `wx.TextCtrl` with optional
bitmap buttons and a drop-down menu.  Controls like this can typically
be found on a toolbar of applications that support some form of search
functionality.  On the Mac this control is implemented using the
native HISearchField control, on the other platforms a generic control
is used, although that may change in the future as more platforms
introduce native search widgets.

If you wish to use a drop-down menu with your wx.SearchCtrl then you
will need to manage its content and handle the menu events yourself,
but this is an easy thing to do.  Simply build the menu, pass it to
`SetMenu`, and also bind a handler for a range of EVT_MENU events.
This gives you the flexibility to use the drop-down menu however you
wish, such as for a history of searches, or as a way to select
different kinds of searches.  The ToolBar.py sample in the demo shows
one way to do this.

Since the control derives from `wx.TextCtrl` it is convenient to use
the styles and events designed for `wx.TextCtrl`.  For example you can
use the ``wx.TE_PROCESS_ENTER`` style and catch the
``wx.EVT_TEXT_ENTER`` event to know when the user has pressed the
Enter key in the control and wishes to start a search.
", "

Events
-------
    =========================    =========================================
    EVT_SEARCHCTRL_SEARCH_BTN    Sent when the search icon is clicked
    EVT_SEARCHCTRL_CANCEL_BTN    Sent when the cancel icon is clicked
    EVT_TEXT                     Sent when the text changes
    EVT_TEXT_ENTER               Sent when the RETURN/ENTER key is pressed 
                                 in the search control and the control has
                                 the wx.TE_PROCESS_ENTER style flag set.
    =========================    =========================================
");


class wxSearchCtrl : public wxTextCtrl
{
public:
    %pythonAppend wxSearchCtrl         "self._setOORInfo(self)";
    %pythonAppend wxSearchCtrl()       "";

    wxSearchCtrl(wxWindow *parent, wxWindowID id=-1,
                 const wxString& value = wxEmptyString,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxPySearchCtrlNameStr);

    DocCtorStrName(
        wxSearchCtrl(),
        "Precreate a wx.SearchCtrl for 2-phase creation.", "",
        PreSearchCtrl);
    
    bool Create(wxWindow *parent, wxWindowID id=-1,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPySearchCtrlNameStr);


    DocDeclStr(
        virtual void , SetMenu( wxMenu* menu ),
        "Sets the search control's menu object. If there is already a menu
associated with the search control it is deleted.", "");
    
    DocDeclStr(
        virtual wxMenu* , GetMenu(),
        "Returns a pointer to the search control's menu object or None if there
is no menu attached.", "");

    
    
    DocDeclStr(
        virtual void , ShowSearchButton( bool show ),
        "Sets the search button visibility value on the search control. If
there is a menu attached, the search button will be visible regardless
of the search button visibility value.  This has no effect in Mac OS X
v10.3", "");
    
    DocDeclStr(
        virtual bool , IsSearchButtonVisible() const,
        "Returns the search button visibility value. If there is a menu
attached, the search button will be visible regardless of the search
button visibility value.  This always returns false in Mac OS X v10.3", "");
    
    DocDeclStr(
        virtual void , ShowCancelButton( bool show ),
        "Shows or hides the cancel button.", "");
    
    DocDeclStr(
        virtual bool , IsCancelButtonVisible() const,
        "Indicates whether the cancel button is visible. ", "");
    

    DocStr(SetSearchBitmap,
           "Sets the bitmap to use for the search button.  This currently does not
work on the Mac.", "");
    DocStr(SetSearchMenuBitmap,
           "Sets the bitmap to use for the search button when there is a drop-down
menu associated with the search control.  This currently does not work
on the Mac.", "");
    DocStr(SetCancelBitmap,
           "Sets the bitmap to use for the cancel button.  This currently does not
work on the Mac.", "");
    
#ifdef __WXMAC__
    %extend {
        void SetSearchBitmap( const wxBitmap& ) {}
        void SetSearchMenuBitmap( const wxBitmap& ) {}
        void SetCancelBitmap( const wxBitmap& ) {}
    }
#else
    void SetSearchBitmap( const wxBitmap& bitmap );
    void SetSearchMenuBitmap( const wxBitmap& bitmap );
    void SetCancelBitmap( const wxBitmap& bitmap );
#endif
    
    
    %property(Menu, GetMenu, SetMenu);
    %property(SearchButtonVisible, IsSearchButtonVisible, ShowSearchButton);
    %property(CancelButtonVisible, IsCancelButtonVisible, ShowCancelButton);
};
     



%constant wxEventType wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN;
%constant wxEventType wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN;

%pythoncode {
    EVT_SEARCHCTRL_CANCEL_BTN = wx.PyEventBinder( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, 1)
    EVT_SEARCHCTRL_SEARCH_BTN = wx.PyEventBinder( wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, 1)
}

//---------------------------------------------------------------------------

