/////////////////////////////////////////////////////////////////////////////
// Name:        drawerg.h
// Purpose:     Generic Drawer child window class.
//              Drawer windows appear under their parent window and
//              behave like a drawer, opening and closing to reveal
//              content that does not need to be visible at all times.
// Author:      Ryan Norton
// Modified by:
// Created:     2004-26-09
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DRAWERG_H_
#define _WX_DRAWERG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "drawerg.h"
#endif

#include "wx/toplevel.h"

class WXDLLEXPORT wxGenericDrawerWindow : public wxTopLevelWindow
{
    DECLARE_DYNAMIC_CLASS(wxGenericDrawerWindow)
    
public:

    wxGenericDrawerWindow();
    
    wxGenericDrawerWindow(wxWindow* parent,
     wxWindowID id,
     const wxString& title,
     wxSize size = wxDefaultSize,
     wxDirection edge = wxLEFT,
     const wxString& name = wxT("drawerwindow"))
    {
        this->Create(parent, id, title, size, edge, name);
    }
     
    ~wxGenericDrawerWindow();
    
    // Create a drawer window. 
    // If parent is NULL, create as a tool window.
    // If parent is not NULL, then wxTopLevelWindow::Attach this window to parent. 
    bool Create(wxWindow *parent,
     wxWindowID id,
     const wxString& title,
     wxSize size = wxDefaultSize,
     wxDirection edge = wxLEFT,
     const wxString& name = wxFrameNameStr);

    bool Open(const bool& show = true); // open or close the drawer, possibility for async param, i.e. animate
    bool Close() { return this->Open(false); }
    bool IsOpen() const;
    
    // Set the edge of the parent where the drawer attaches.
    bool SetPreferredEdge(const wxDirection& edge);
    wxDirection GetPreferredEdge() const;
    wxDirection GetCurrentEdge() const;	// not necessarily the preferred, due to screen constraints
        
protected:
    bool 		m_bOpen;
    wxDirection m_nCurrentEdge;
    wxDirection m_nPreferredEdge;
    int			m_nOpenOffset;
    class wxGenericDrawerTimer* m_pTimer;
    friend class wxGenericDrawerTimer;
    wxSize		m_LastParentSize;
    
    void OnDrawerFocus(class wxFocusEvent& evt);
    void OnDrawerMove(class wxMoveEvent& evt);
    void OnDrawerSize(class wxSizeEvent& evt);

    void DoDrawerPosition();
    void DoDrawerSize();
};

#endif // _WX_DRAWERG_H_