/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/toolbar.h
// Purpose:     wxToolBar class
// Author:      Julian Smart
// Modified by: 13.12.99 by VZ during toolbar classes reorganization
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOOLBAR_H_
#define _WX_TOOLBAR_H_

#ifdef __GNUG__
    #pragma interface "toolbar.h"
#endif

class WXDLLEXPORT wxToolBar : public wxToolBarBase
{
public:
    // ctors and dtor
    wxToolBar() { Init(); }

    wxToolBar(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxNO_BORDER | wxTB_HORIZONTAL,
                const wxString& name = wxToolBarNameStr)
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxNO_BORDER | wxTB_HORIZONTAL,
                const wxString& name = wxToolBarNameStr);

    virtual ~wxToolBar();

    // override/implement base class virtuals
    virtual wxToolBarTool *FindToolForPosition(wxCoord x, wxCoord y) const;

    virtual bool Realize();

    // implementation from now on

    // find tool by widget
    wxToolBarTool *FindToolByWidget(WXWidget w) const;

protected:
    // common part of all ctors
    void Init();

    // implement base class pure virtuals
    virtual bool DoInsertTool(size_t pos, wxToolBarTool *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarTool *tool);

    virtual void DoEnableTool(wxToolBarTool *tool, bool enable);
    virtual void DoToggleTool(wxToolBarTool *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarTool *tool, bool toggle);

private:
    DECLARE_DYNAMIC_CLASS(wxToolBar)
};

#endif
    // _WX_TOOLBAR_H_
