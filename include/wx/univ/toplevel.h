/////////////////////////////////////////////////////////////////////////////
// Name:        wx/toplevel.h
// Purpose:     Top level window, abstraction of wxFrame and wxDialog
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __WX_UNIV_TOPLEVEL_H__
#define __WX_UNIV_TOPLEVEL_H__

#ifdef __GNUG__
    #pragma interface "univtoplevel.h"
#endif

#include "wx/univ/inpcons.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// frame decorations type flags used in wxRenderer and wxColourScheme
enum
{
    wxTOPLEVEL_BORDER          = 0x00000001,
    wxTOPLEVEL_MAXIMIZED       = 0x00000002,
    wxTOPLEVEL_TITLEBAR        = 0x00000004,
    wxTOPLEVEL_RESIZEABLE      = 0x00000008,
    wxTOPLEVEL_ICON            = 0x00000010,
    wxTOPLEVEL_CLOSE_BUTTON    = 0x00000020,
    wxTOPLEVEL_MAXIMIZE_BUTTON = 0x00000040,
    wxTOPLEVEL_MINIMIZE_BUTTON = 0x00000080,
    wxTOPLEVEL_RESTORE_BUTTON  = 0x00000100,
    wxTOPLEVEL_HELP_BUTTON     = 0x00000200,    
    wxTOPLEVEL_ACTIVE          = 0x00000400
};

//-----------------------------------------------------------------------------
// wxTopLevelWindow
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxTopLevelWindow : public wxTopLevelWindowNative
{
public:
    // construction
    wxTopLevelWindow() { Init(); }
    wxTopLevelWindow(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE,
                     const wxString& name = wxFrameNameStr)
    {
        Init();

        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    // implement base class pure virtuals
    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);
    virtual wxPoint GetClientAreaOrigin() const;
    virtual void DoGetClientSize(int *width, int *height) const;
    virtual void DoSetClientSize(int width, int height);
    virtual void SetIcon(const wxIcon& icon);

    // implementation from now on
    // --------------------------

protected:
    // common part of all ctors
    void Init();

    // return wxTOPLEVEL_xxx combination based on current state of the frame
    long GetDecorationsStyle() const;
    
    DECLARE_DYNAMIC_CLASS(wxTopLevelWindow)
    DECLARE_EVENT_TABLE()
    void OnNcPaint(wxPaintEvent& event);

    // TRUE if wxTLW should render decorations (aka titlebar) itself
    static int ms_drawDecorations;
    // true for currently active frame
    bool m_isActive:1;
    // version of icon for titlebar (16x16)
    wxIcon m_titlebarIcon;
    
};

#endif // __WX_UNIV_TOPLEVEL_H__
