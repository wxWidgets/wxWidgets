/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/panelg.h
// Purpose:     wxPanel: a container for child controls
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_PANEL_H_
#define _WX_GENERIC_PANEL_H_

#ifdef __GNUG__
    #pragma interface "panelg.h"
#endif

// ----------------------------------------------------------------------------
// headers and forward declarations
// ----------------------------------------------------------------------------

#include "wx/window.h"

class WXDLLEXPORT wxButton;

WXDLLEXPORT_DATA(extern const wxChar*) wxPanelNameStr;

// ----------------------------------------------------------------------------
// wxPanel contains other controls and implements TAB traversal between them
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxPanel : public wxWindow
{
public:
    wxPanel() { Init(); }

    // Old-style constructor (no default values for coordinates to avoid
    // ambiguity with the new one)
    wxPanel(wxWindow *parent,
            int x, int y, int width, int height,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr)
    {
        Init();

        Create(parent, -1, wxPoint(x, y), wxSize(width, height), style, name);
    }

    // Constructor
    wxPanel(wxWindow *parent,
            wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr)
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    // Pseudo ctor
    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxPanelNameStr);

    // Sends an OnInitDialog event, which in turns transfers data to
    // to the dialog via validators.
    virtual void InitDialog();

    // a default button is activated when Enter is pressed
    wxButton *GetDefaultItem() const { return m_btnDefault; }
    void SetDefaultItem(wxButton *btn) { m_btnDefault = btn; }

    // implementation from now on
    // --------------------------

        // responds to colour changes
    void OnSysColourChanged(wxSysColourChangedEvent& event);

        // process a keyboard navigation message (Tab traversal)
    void OnNavigationKey(wxNavigationKeyEvent& event);

        // set the focus to the first child if we get it
    void OnFocus(wxFocusEvent& event);

        // calls layout for layout constraints and sizers
    void OnSize(wxSizeEvent& event);

        // overridden to tab move focus into first focusable child
    virtual void SetFocus();

        // called by wxWindow whenever it gets focus
    void SetLastFocus(wxWindow *win) { m_winLastFocused = win; }
    wxWindow *GetLastFocus() const { return m_winLastFocused; }

protected:
    // common part of all ctors
    void Init();

    // set the focus to the child which had it the last time
    bool SetFocusToChild();

    // the child which had the focus last time this panel was activated
    wxWindow *m_winLastFocused;

    // a default button or NULL
    wxButton *m_btnDefault;

private:
    DECLARE_DYNAMIC_CLASS(wxPanel)
    DECLARE_EVENT_TABLE()
};

// this function is for wxWindows use only
extern bool wxSetFocusToChild(wxWindow *win, wxWindow **child);

#endif
    // _WX_GENERIC_PANEL_H_
