/////////////////////////////////////////////////////////////////////////////
// Name:        univ/checklst.cpp
// Purpose:     wxCheckListBox implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     12.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univchecklst.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CHECKLISTBOX

#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/dcclient.h"
    #include "wx/checklst.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation of wxCheckListBox
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxCheckListBox, wxListBox)

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxCheckListBox::Init()
{
}

// ----------------------------------------------------------------------------
// wxCheckListBox functions
// ----------------------------------------------------------------------------

bool wxCheckListBox::IsChecked(size_t item) const
{
    wxCHECK_MSG( item < m_checks.GetCount(), FALSE,
                 _T("invalid index in wxCheckListBox::IsChecked") );

    return m_checks[item] != 0;
}

void wxCheckListBox::Check(size_t item, bool check)
{
    wxCHECK_RET( item < m_checks.GetCount(),
                 _T("invalid index in wxCheckListBox::Check") );

    // intermediate var is needed to avoid compiler warning with VC++
    bool isChecked = m_checks[item] != 0;
    if ( check != isChecked )
    {
        m_checks[item] = check;

        RefreshItem(item);
    }
}

// ----------------------------------------------------------------------------
// methods forwarded to wxListBox
// ----------------------------------------------------------------------------

void wxCheckListBox::Delete(int n)
{
    wxCHECK_RET( n < GetCount(), _T("invalid index in wxListBox::Delete") );

    wxListBox::Delete(n);

    m_checks.RemoveAt(n);
}

int wxCheckListBox::DoAppend(const wxString& item)
{
    int pos = wxListBox::DoAppend(item);

    // the item is initially unchecked
    m_checks.Insert(FALSE, pos);

    return pos;
}

void wxCheckListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    wxListBox::DoInsertItems(items, pos);

    size_t count = items.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_checks.Insert(FALSE, pos + n);
    }
}

void wxCheckListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    // call it first as it does DoClear()
    wxListBox::DoSetItems(items, clientData);

    size_t count = items.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_checks.Add(FALSE);
    }
}

void wxCheckListBox::DoClear()
{
    m_checks.Empty();
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

wxSize wxCheckListBox::DoGetBestClientSize() const
{
    wxSize size = wxListBox::DoGetBestClientSize();
    size.x += GetRenderer()->GetCheckBitmapSize().x;

    return size;
}

void wxCheckListBox::DoDrawRange(wxControlRenderer *renderer,
                                 int itemFirst, int itemLast)
{
    renderer->DrawCheckItems(this, itemFirst, itemLast);
}

// ----------------------------------------------------------------------------
// actions
// ----------------------------------------------------------------------------

wxString wxCheckListBox::GetInputHandlerType() const
{
    return wxINP_HANDLER_CHECKLISTBOX;
}

bool wxCheckListBox::PerformAction(const wxControlAction& action,
                                   long numArg,
                                   const wxString& strArg)
{
    if ( action == wxACTION_CHECKLISTBOX_TOGGLE )
    {
        int sel = (int)numArg;
        if ( sel == -1 )
        {
            sel = GetSelection();
        }

        if ( sel != -1 )
        {
            Check(sel, !IsChecked(sel));

            SendEvent(sel, wxEVT_COMMAND_CHECKLISTBOX_TOGGLED);
        }
    }
    else
    {
        return wxListBox::PerformAction(action, numArg, strArg);
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxStdCheckListboxInputHandler
// ----------------------------------------------------------------------------

wxStdCheckListboxInputHandler::
wxStdCheckListboxInputHandler(wxInputHandler *inphand)
    : wxStdListboxInputHandler(inphand)
{
}

bool wxStdCheckListboxInputHandler::HandleKey(wxControl *control,
                                              const wxKeyEvent& event,
                                              bool pressed)
{
    if ( pressed && (event.GetKeyCode() == WXK_SPACE) )
        control->PerformAction(wxACTION_CHECKLISTBOX_TOGGLE);

    return wxStdListboxInputHandler::HandleKey(control, event, pressed);
}

bool wxStdCheckListboxInputHandler::HandleMouse(wxControl *control,
                                                const wxMouseEvent& event)
{
    if ( event.LeftDown() || event.LeftDClick() )
    {
        wxCheckListBox *lbox = wxStaticCast(control, wxCheckListBox);
        int x, y;

        const wxPoint& pt = event.GetPosition();
        lbox->CalcUnscrolledPosition(pt.x, pt.y, &x, &y);

        wxRenderer *renderer = lbox->GetRenderer();
        x -= renderer->GetCheckItemMargin();

        int item = y / lbox->GetLineHeight();
        if ( x >= 0 &&
             x < renderer->GetCheckBitmapSize().x &&
             item >= 0 &&
             item < lbox->GetCount() )
        {
            lbox->PerformAction(wxACTION_CHECKLISTBOX_TOGGLE, item);

            return TRUE;
        }
    }

    return wxStdListboxInputHandler::HandleMouse(control, event);
}

#endif // wxUSE_CHECKLISTBOX
