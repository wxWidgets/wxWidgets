/////////////////////////////////////////////////////////////////////////////
// Name:        edtree.cpp
// Purpose:     Resource editor project management tree
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "edtree.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"

#include "wx/checkbox.h"
#include "wx/button.h"
#include "wx/choice.h"
#include "wx/listbox.h"
#include "wx/radiobox.h"
#include "wx/statbox.h"
#include "wx/gauge.h"
#include "wx/slider.h"
#include "wx/textctrl.h"
#endif

#include "edtree.h"
#include "reseditr.h"

BEGIN_EVENT_TABLE(wxResourceEditorProjectTree, wxTreeCtrl)
    EVT_LEFT_DCLICK(wxResourceEditorProjectTree::LeftDClick)
    EVT_TREE_SEL_CHANGED(IDC_TREECTRL, wxResourceEditorProjectTree::OnSelChanged)
END_EVENT_TABLE()

wxResourceEditorProjectTree::wxResourceEditorProjectTree(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
        long style):
     wxTreeCtrl(parent, id, pos, size, style)
{
}

void wxResourceEditorProjectTree::LeftDClick(wxMouseEvent& WXUNUSED(event))
{
#if 0
    long sel = GetSelection();
    if (sel == -1)
        return;

    if (GetItemData(sel) == 0)
        return;

    wxItemResource* res = (wxResourceTreeData *)GetItemData(sel)->GetResource();
    wxString resType(res->GetType());
    if (resType != "wxDialog" && resType != "wxDialogBox" && resType != "wxPanel")
        return;

    wxResourceEditorFrame *frame = (wxResourceEditorFrame *)wxWindow::GetParent();
    wxResourceManager *manager = frame->manager;

    manager->EditSelectedResource();
#endif
}

void wxResourceEditorProjectTree::OnSelChanged(wxTreeEvent& WXUNUSED(event))
{
    long sel = GetSelection();
    if (sel == -1)
        return;

    if (GetItemData(sel) == 0)
        return;

    if (m_invalid)
        return;

    wxItemResource* res = ((wxResourceTreeData *)GetItemData(sel))->GetResource();
    wxString resType(res->GetType());
    if (resType != "wxDialog" && resType != "wxDialogBox" && resType != "wxPanel")
        return;

    wxResourceManager::GetCurrentResourceManager()->Edit(res);
}

