/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/toolbar.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/toolbar.h"

wxToolBar::wxToolBar()
{
}

wxToolBar::wxToolBar(wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
}

bool wxToolBar::Create(wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    return false;
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    return NULL;
}

bool wxToolBar::DoInsertTool(size_t pos, wxToolBarToolBase *tool)
{
    return false;
}

bool wxToolBar::DoDeleteTool(size_t pos, wxToolBarToolBase *tool)
{
    return false;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *tool, bool enable)
{
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *tool, bool toggle)
{
}

void wxToolBar::DoSetToggle(wxToolBarToolBase *tool, bool toggle)
{
}

wxToolBarToolBase *wxToolBar::CreateTool(int toolid,
                                      const wxString& label,
                                      const wxBitmap& bmpNormal,
                                      const wxBitmap& bmpDisabled,
                                      wxItemKind kind,
                                      wxObject *clientData,
                                      const wxString& shortHelp,
                                      const wxString& longHelp)
{
    return NULL;
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control,
                                          const wxString& label)
{
    return NULL;
}

