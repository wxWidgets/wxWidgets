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
#include "wx/univ/inphand.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// frame decorations type flags used in wxRenderer and wxColourScheme
// (also used for hit tests)
enum
{
    wxTOPLEVEL_ACTIVE          = 0x00000001,
    wxTOPLEVEL_MAXIMIZED       = 0x00000002,
    wxTOPLEVEL_TITLEBAR        = 0x00000004,
    wxTOPLEVEL_RESIZEABLE      = 0x00000008,
    wxTOPLEVEL_ICON            = 0x00000010,
    wxTOPLEVEL_BUTTON_CLOSE    = 0x00000020,
    wxTOPLEVEL_BUTTON_MAXIMIZE = 0x00000040,
    wxTOPLEVEL_BUTTON_MINIMIZE = 0x00000080,
    wxTOPLEVEL_BUTTON_RESTORE  = 0x00000100,
    wxTOPLEVEL_BUTTON_HELP     = 0x00000200,    
    wxTOPLEVEL_BORDER          = 0x00000400,
};

// frame hit test return values:
enum
{
    wxHT_TOPLEVEL_NOWHERE = 0,
    wxHT_TOPLEVEL_CLIENT_AREA,
    wxHT_TOPLEVEL_ICON,
    wxHT_TOPLEVEL_TITLEBAR,
    wxHT_TOPLEVEL_BUTTON_CLOSE = wxTOPLEVEL_BUTTON_CLOSE,
    wxHT_TOPLEVEL_BUTTON_MAXIMIZE = wxTOPLEVEL_BUTTON_MAXIMIZE,
    wxHT_TOPLEVEL_BUTTON_MINIMIZE = wxTOPLEVEL_BUTTON_MINIMIZE,
    wxHT_TOPLEVEL_BUTTON_RESTORE = wxTOPLEVEL_BUTTON_RESTORE,
    wxHT_TOPLEVEL_BUTTON_HELP = wxTOPLEVEL_BUTTON_HELP,
    wxHT_TOPLEVEL_BORDER_N,
    wxHT_TOPLEVEL_BORDER_S,
    wxHT_TOPLEVEL_BORDER_E,
    wxHT_TOPLEVEL_BORDER_W,
    wxHT_TOPLEVEL_BORDER_NE = wxHT_TOPLEVEL_BORDER_N | wxHT_TOPLEVEL_BORDER_E,
    wxHT_TOPLEVEL_BORDER_SE = wxHT_TOPLEVEL_BORDER_S | wxHT_TOPLEVEL_BORDER_E,
    wxHT_TOPLEVEL_BORDER_NW = wxHT_TOPLEVEL_BORDER_N | wxHT_TOPLEVEL_BORDER_W,
    wxHT_TOPLEVEL_BORDER_SW = wxHT_TOPLEVEL_BORDER_S | wxHT_TOPLEVEL_BORDER_W,
};

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

#define wxACTION_TOPLEVEL_ACTIVATE     _T("activate")   // (de)activate the frame
#define wxACTION_TOPLEVEL_CLOSE        _T("close")      // close the frame
#define wxACTION_TOPLEVEL_MAXIMIZE     _T("maximize")   // maximize the frame
#define wxACTION_TOPLEVEL_MINIMIZE     _T("minimize")   // minimize the frame
#define wxACTION_TOPLEVEL_RESTORE      _T("restore")    // undo maximization
#define wxACTION_TOPLEVEL_CONTEXT_HELP _T("contexthelp")// context help mode

//-----------------------------------------------------------------------------
// wxTopLevelWindow
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxTopLevelWindow : public wxTopLevelWindowNative,
                                     public wxInputConsumer
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
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = -1,
                               const wxString& strArg = wxEmptyString);
    virtual wxWindow *GetInputWindow() const { return (wxWindow*)this; }

    // common part of all ctors
    void Init();

    // return wxTOPLEVEL_xxx combination based on current state of the frame
    long GetDecorationsStyle() const;
    
    void OnNcPaint(wxPaintEvent& event);

    // TRUE if wxTLW should render decorations (aka titlebar) itself
    static int ms_drawDecorations;
    // true for currently active frame
    bool m_isActive:1;
    // version of icon for titlebar (16x16)
    wxIcon m_titlebarIcon;
    // saved window style in fullscreen mdoe
    long m_fsSavedStyle;

    DECLARE_DYNAMIC_CLASS(wxTopLevelWindow)
    DECLARE_EVENT_TABLE()
    WX_DECLARE_INPUT_CONSUMER()
};

// ----------------------------------------------------------------------------
// wxStdFrameInputHandler: handles focus, resizing and titlebar buttons clicks
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdFrameInputHandler : public wxStdInputHandler
{
public:
    wxStdFrameInputHandler(wxInputHandler *inphand);

    virtual bool HandleMouse(wxInputConsumer *consumer,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxInputConsumer *consumer, const wxMouseEvent& event);
    virtual bool HandleActivation(wxInputConsumer *consumer, bool activated);
};

#endif // __WX_UNIV_TOPLEVEL_H__
