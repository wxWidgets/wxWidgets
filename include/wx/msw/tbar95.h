/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/tbar95.h
// Purpose:     wxToolBar95 (Windows 95 toolbar) class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TBAR95_H_
#define _WX_TBAR95_H_

#ifdef __GNUG__
#pragma interface "tbar95.h"
#endif

#if wxUSE_TOOLBAR

#include "wx/tbarbase.h"

class WXDLLEXPORT wxToolBar95 : public wxToolBarBase
{
public:
    wxToolBar95() { Init(); }

    wxToolBar95(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxNO_BORDER | wxTB_HORIZONTAL,
                const wxString& name = wxToolBarNameStr)
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    ~wxToolBar95();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxNO_BORDER | wxTB_HORIZONTAL,
                const wxString& name = wxToolBarNameStr);

    // override base class virtuals

    virtual wxToolBarTool *AddTool(int toolIndex,
                                   const wxBitmap& bitmap,
                                   const wxBitmap& pushedBitmap = wxNullBitmap,
                                   bool toggle = FALSE,
                                   long xPos = -1, long yPos = -1,
                                   wxObject *clientData = NULL,
                                   const wxString& helpString1 = wxEmptyString,
                                   const wxString& helpString2 = wxEmptyString);
    virtual bool AddControl(wxControl *control);

    virtual void ClearTools();

    virtual bool Realize() { return CreateTools(); };

    virtual void EnableTool(int toolIndex, bool enable);
    virtual void ToggleTool(int toolIndex, bool toggle);

    virtual void SetToolBitmapSize(const wxSize& size);
    virtual wxSize GetToolSize() const;
    virtual wxSize GetMaxSize() const;

    virtual bool GetToolState(int toolIndex) const;

    virtual bool CreateTools();
    virtual void SetRows(int nRows);

    // IMPLEMENTATION
    virtual bool MSWCommand(WXUINT param, WXWORD id);
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);

    void OnMouseEvent(wxMouseEvent& event);
    void OnSysColourChanged(wxSysColourChangedEvent& event);

protected:
    // common part of all ctors
    void Init();

    WXHBITMAP m_hBitmap;

    DECLARE_EVENT_TABLE()
};

class WXDLLEXPORT wxToolBar : public wxToolBar95
{
public:
    wxToolBar() { }

    wxToolBar(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxNO_BORDER | wxTB_HORIZONTAL,
              const wxString& name = wxToolBarNameStr)
        : wxToolBar95(parent, id, pos, size, style, name)
    {
    }

private:
    DECLARE_DYNAMIC_CLASS(wxToolBar)
};

#endif // wxUSE_TOOLBAR

#endif
    // _WX_TBAR95_H_
