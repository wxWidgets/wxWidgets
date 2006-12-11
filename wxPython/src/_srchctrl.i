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


MustHaveApp(wxSearchCtrl);


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


    // get/set search button menu
    // --------------------------
    virtual void SetMenu( wxMenu* menu );
    virtual wxMenu* GetMenu();
    
    // get/set search options
    // ----------------------
    virtual void SetSearchButtonVisible( bool show );
    virtual bool GetSearchButtonVisible() const;

    virtual void SetCancelButtonVisible( bool show );
    virtual bool GetCancelButtonVisible() const;

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
    %property(SearchButtonVisible, GetSearchButtonVisible, SetSearchButtonVisible);
    %property(CancelButtonVisible, GetCancelButtonVisible, SetCancelButtonVisible);
};
     



%constant wxEventType wxEVT_COMMAND_SEARCHCTRL_CANCEL;
%constant wxEventType wxEVT_COMMAND_SEARCHCTRL_SEARCH;

%pythoncode {
    EVT_SEARCHCTRL_CANCEL = wx.PyEventBinder( wxEVT_COMMAND_SEARCHCTRL_CANCEL, 1)
    EVT_SEARCHCTRL_SEARCH = wx.PyEventBinder( wxEVT_COMMAND_SEARCHCTRL_SEARCH, 1)
}

//---------------------------------------------------------------------------

