/////////////////////////////////////////////////////////////////////////////
// Name:        univ/checklst.cpp
// Purpose:     wxCheckListBox implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     12.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

wxCheckListBox::wxCheckListBox(wxWindow *parent,
                               wxWindowID id,
                               const wxPoint &pos,
                               const wxSize &size,
                               const wxArrayString& choices,
                               long style,
                               const wxValidator& validator,
                               const wxString &name)
{
    Init();

    Create(parent, id, pos, size, choices, style, validator, name);
}

bool wxCheckListBox::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxPoint &pos,
                            const wxSize &size,
                            const wxArrayString& choices,
                            long style,
                            const wxValidator& validator,
                            const wxString &name)
{
    wxCArrayString chs(choices);

    return Create(parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

bool wxCheckListBox::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxPoint &pos,
                            const wxSize &size,
                            int n,
                            const wxString choices[],
                            long style,
                            const wxValidator& validator,
                            const wxString &name)
{
    if ( !wxListBox::Create(parent, id, pos, size,
                            n, choices, style, validator, name) )
        return false;

    CreateInputHandler(wxINP_HANDLER_CHECKLISTBOX);

    return true;
}

// ----------------------------------------------------------------------------
// wxCheckListBox functions
// ----------------------------------------------------------------------------

bool wxCheckListBox::IsChecked(size_t item) const
{
    wxCHECK_MSG( item < m_checks.GetCount(), false,
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
    m_checks.Insert(false, pos);

    return pos;
}

void wxCheckListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    wxListBox::DoInsertItems(items, pos);

    size_t count = items.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_checks.Insert(false, pos + n);
    }
}

void wxCheckListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    // call it first as it does DoClear()
    wxListBox::DoSetItems(items, clientData);

    size_t count = items.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_checks.Add(false);
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

            SendEvent(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, sel);
        }
    }
    else
    {
        return wxListBox::PerformAction(action, numArg, strArg);
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxStdCheckListboxInputHandler
// ----------------------------------------------------------------------------

wxStdCheckListboxInputHandler::
wxStdCheckListboxInputHandler(wxInputHandler *inphand)
    : wxStdListboxInputHandler(inphand)
{
}

bool wxStdCheckListboxInputHandler::HandleKey(wxInputConsumer *consumer,
                                              const wxKeyEvent& event,
                                              bool pressed)
{
    if ( pressed && (event.GetKeyCode() == WXK_SPACE) )
        consumer->PerformAction(wxACTION_CHECKLISTBOX_TOGGLE);

    return wxStdListboxInputHandler::HandleKey(consumer, event, pressed);
}

bool wxStdCheckListboxInputHandler::HandleMouse(wxInputConsumer *consumer,
                                                const wxMouseEvent& event)
{
    if ( event.LeftDown() || event.LeftDClick() )
    {
        wxCheckListBox *lbox = wxStaticCast(consumer->GetInputWindow(), wxCheckListBox);
        int x, y;

        wxPoint pt = event.GetPosition();
        pt -= consumer->GetInputWindow()->GetClientAreaOrigin();
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

            return true;
        }
    }

    return wxStdListboxInputHandler::HandleMouse(consumer, event);
}

#endif // wxUSE_CHECKLISTBOX
