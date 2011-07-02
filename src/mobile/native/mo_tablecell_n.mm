/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_tablecell_n.mm
// Purpose:     wxMoTableCell class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/panel.h"
    #include "wx/bmpbuttn.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
#endif

#include "wx/dcbuffer.h"

#include "wx/osx/private.h"
#include "wx/mobile/native/tablectrl.h"
#include "wx/mobile/native/tablecell.h"
#include "wx/mobile/native/settings.h"


#pragma mark wxMoTableCell

IMPLEMENT_CLASS(wxMoTableCell, wxObject)

wxMoTableCell::wxMoTableCell(wxMoTableCtrl* ctrl, const wxString& reuseName, wxMoTableCellStyle cellStyle)
{
    SetCellWidgetImpl(wxWidgetImpl::CreateTableViewCell( this ));
}

wxMoTableCell::~wxMoTableCell()
{
    // FIXME stub
}

void wxMoTableCell::Init()
{
    m_widgetImpl = NULL;
}

void wxMoTableCell::Copy(const wxMoTableCell& cell)
{
    // FIXME stub
}

bool wxMoTableCell::CreateContentWindow(wxMoTableCtrl* ctrl)
{
    // FIXME stub

    return true;
}

// Sets the accessory window
void wxMoTableCell::SetAccessoryWindow(wxWindow* win)
{
    // FIXME stub
}

// Sets the editing accessory window
void wxMoTableCell::SetEditingAccessoryWindow(wxWindow* win)
{
    // FIXME stub
}

// Sets editing mode (not yet implemented).
bool wxMoTableCell::SetEditingMode(bool editingMode, bool WXUNUSED(animated))
{
    // FIXME stub
    // TODO

    return true;
}

// Is the delete confirmation button showing for this path?
bool wxMoTableCell::IsDeleteButtonShowing(wxMoTableCtrl* tableCtrl) const
{
    // FIXME stub

    return true;
}

// Prepares the cell for reuse
void wxMoTableCell::PrepareForReuse(wxMoTableCtrl* WXUNUSED(tableCtrl))
{
    // FIXME stub
}


#pragma mark wxMoTableCellContentWindow

IMPLEMENT_DYNAMIC_CLASS(wxMoTableCellContentWindow, wxWindow)

BEGIN_EVENT_TABLE(wxMoTableCellContentWindow, wxWindow)
    EVT_PAINT(wxMoTableCellContentWindow::OnPaint)
    EVT_MOUSE_EVENTS(wxMoTableCellContentWindow::OnMouseEvents)
END_EVENT_TABLE()

bool wxMoTableCellContentWindow::Create(wxWindow* parent, wxWindowID id,
        const wxPoint& pos, const wxSize& sz, long style)
{
    // FIXME stub

    return true;
}

void wxMoTableCellContentWindow::Init()
{
    // FIXME stub
}

void wxMoTableCellContentWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoTableCellContentWindow::OnMouseEvents(wxMouseEvent& event)
{
    // FIXME stub
}
