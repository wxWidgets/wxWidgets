/////////////////////////////////////////////////////////////////////////////
// Name:        panelg.h
// Purpose:     wxPanel: similar to wxWindows but is coloured as for a dialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __PANELH_G__
#define __PANELH_G__

#ifdef __GNUG__
#pragma interface "panelg.h"
#endif

#include "wx/window.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxPanelNameStr;


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

        // called by wxWindow whenever it gets focus
    void SetLastFocus(long focus) { m_lastFocus = focus; }
    long GetLastFocus() const { return m_lastFocus; }

protected:
    // common part of all ctors
    void Init();

    // the child which had the focus last time this panel was activated
    long m_lastFocus;

    // a default button or NULL
    wxButton *m_btnDefault;

private:
    DECLARE_DYNAMIC_CLASS(wxPanel)
    DECLARE_EVENT_TABLE()
};

#endif
    // __PANELH_G__
