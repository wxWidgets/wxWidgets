/////////////////////////////////////////////////////////////////////////////
// Name:        _collpane.i
// Purpose:     SWIG interface for wxCollapsiblePane
//
// Author:      Robin Dunn
//
// Created:     10-Nov-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{
#include <wx/collpane.h>
%}

MAKE_CONST_WXSTRING(CollapsiblePaneNameStr);

enum {
     wxCP_DEFAULT_STYLE,
     wxCP_NO_TLW_RESIZE
};



MustHaveApp(wxCollapsiblePane);
DocStr(wxCollapsiblePane,
"A collapsable pane is a container with an embedded button-like
control which can be used by the user to collapse or expand the pane's
contents.

Once constructed you should use the `GetPane` function to access the
pane and add your controls inside it (i.e. use the window returned
from `GetPane` as the parent for the controls which must go in the
pane, NOT the wx.CollapsiblePane itself!).

Note that because of its nature of control which can dynamically (and
drastically) change its size at run-time under user-input, when
putting a wx.CollapsiblePane inside a `wx.Sizer` you should be careful
to add it with a proportion value of zero; this is because otherwise
all other windows with non-zero proportion values would automatically
get resized each time the user expands or collapses the pane window,
usually resulting a weird, flickering effect.", "");

class wxCollapsiblePane : public wxControl
{
public:
    %pythonAppend wxCollapsiblePane         "self._setOORInfo(self)";
    %pythonAppend wxCollapsiblePane()       "";

    DocCtorStr(
        wxCollapsiblePane(wxWindow *parent,
                          wxWindowID winid = -1,
                          const wxString& label = wxPyEmptyString,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = wxCP_DEFAULT_STYLE,
                          const wxValidator& val = wxDefaultValidator,
                          const wxString& name = wxPyCollapsiblePaneNameStr),
        "Create and show a wx.CollapsiblePane", "");
    
    DocCtorStrName(
        wxCollapsiblePane(),
        "Precreate a wx.CollapsiblePane for 2-phase creation.", "",
        PreCollapsiblePane);


    DocDeclStr(
        bool , Create(wxWindow *parent,
                      wxWindowID winid =-1,
                      const wxString& label = wxPyEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxCP_DEFAULT_STYLE,
                      const wxValidator& val = wxDefaultValidator,
                      const wxString& name = wxPyCollapsiblePaneNameStr),
        "", "");
    


    
    DocDeclStr(
        virtual void , Collapse(bool collapse = true),
        "Collapses or expands the pane window.", "");
    
    DocDeclStr(
        void , Expand(),
        "Same as Collapse(False).", "");
    

    DocDeclStr(
        virtual bool , IsCollapsed() const,
        "Returns ``True`` if the pane window is currently hidden.", "");
    
    DocDeclStr(
        bool , IsExpanded() const,
        "Returns ``True`` if the pane window is currently shown.", "");
    

    DocDeclStr(
        virtual wxWindow *, GetPane() const,
        "Returns a reference to the pane window.  Use the returned `wx.Window`
as the parent of widgets to make them part of the collapsible area.", "");
    
};



//---------------------------------------------------------------------------


%constant wxEventType wxEVT_COMMAND_COLLPANE_CHANGED;
%pythoncode {
    EVT_COLLAPSIBLEPANE_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_COLLPANE_CHANGED, 1 )
}

class wxCollapsiblePaneEvent : public wxCommandEvent
{
public:
    //wxCollapsiblePaneEvent() {}
    wxCollapsiblePaneEvent(wxObject *generator, int id, bool collapsed);

    bool GetCollapsed() const;
    void SetCollapsed(bool c);
};

//---------------------------------------------------------------------------
