/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/tbarmsw.h
// Purpose:     wxToolBar for Win16
// Author:      Julian Smart
// Modified by: 13.12.99 by VZ during toolbar classes reorganization
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TBARMSW_H_
#define _WX_TBARMSW_H_

#ifdef __GNUG__
#pragma interface "tbarmsw.h"
#endif

#if wxUSE_BUTTONBAR && wxUSE_TOOLBAR

#include "wx/tbarbase.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxButtonBarNameStr;

class WXDLLEXPORT wxMemoryDC;

// ----------------------------------------------------------------------------
// wxToolBar for Win16
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxToolBar : public wxToolBarBase
{
public:
    // construction
    wxToolBar() { Init(); }

    wxToolBar(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxNO_BORDER|wxTB_HORIZONTAL,
              const wxString& name = wxButtonBarNameStr)
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxNO_BORDER|wxTB_HORIZONTAL,
                const wxString& name = wxButtonBarNameStr);

    ~wxToolBar();

    // implement/override base class (pure) virtuals
    virtual wxToolBarToolBase *AddTool(int id,
                                       const wxBitmap& bitmap,
                                       const wxBitmap& pushedBitmap,
                                       bool toggle,
                                       wxCoord xPos,
                                       wxCoord yPos = -1,
                                       wxObject *clientData = NULL,
                                       const wxString& helpString1 = wxEmptyString,
                                       const wxString& helpString2 = wxEmptyString);

    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const;

    virtual void SetToolBitmapSize(const wxSize& size);
    virtual wxSize GetToolSize() const;

    virtual bool Realize();

    // implementation only from now on
    // -------------------------------

    // Handle wxWindows events
    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);

protected:
    void Init();

    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool);
    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable);
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle);

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxBitmap& bitmap1,
                                          const wxBitmap& bitmap2,
                                          bool toggle,
                                          wxObject *clientData,
                                          const wxString& shortHelpString,
                                          const wxString& longHelpString);
    virtual wxToolBarToolBase *CreateTool(wxControl *control);

    void DoRedrawTool(wxToolBarToolBase *tool);

    void DrawTool(wxDC& dc, wxToolBarToolBase *tool);
    void DrawTool(wxDC& dc, wxToolBarToolBase *tool, int state);

    void GetSysColors();
    bool InitGlobalObjects();
    void FreeGlobalObjects();
    void PatB(WXHDC hdc,int x,int y,int dx,int dy, long rgb);
    void CreateMask(WXHDC hDC, int xoffset, int yoffset, int dx, int dy);
    void DrawBlankButton(WXHDC hdc, int x, int y, int dx, int dy, int state);
    void DrawButton(WXHDC hdc, int x, int y, int dx, int dy,
                    wxToolBarToolBase *tool, int state);
    WXHBITMAP CreateDitherBitmap();
    bool CreateDitherBrush();
    bool FreeDitherBrush();
    WXHBITMAP CreateMappedBitmap(WXHINSTANCE hInstance, void *lpBitmapInfo);
    WXHBITMAP CreateMappedBitmap(WXHINSTANCE hInstance, WXHBITMAP hBitmap);

    int               m_currentRowsOrColumns;
    int               m_pressedTool, m_currentTool;

    wxCoord           m_xPos, m_yPos;
    wxCoord           m_lastX, m_lastY;

    WXHBRUSH          m_hbrDither;
    WXDWORD           m_rgbFace;
    WXDWORD           m_rgbShadow;
    WXDWORD           m_rgbHilight;
    WXDWORD           m_rgbFrame;

    //
    // m_hdcMono is the DC that holds a mono bitmap, m_hbmMono
    // that is used to create highlights
    // of button faces.
    // m_hbmDefault hold the default bitmap if there is one.
    //
    WXHDC             m_hdcMono;
    WXHBITMAP         m_hbmMono;
    WXHBITMAP         m_hbmDefault;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxToolBar)
};

#endif // wxUSE_TOOL/BUTTONBAR

#endif
    // _WX_TBARMSW_H_
