/////////////////////////////////////////////////////////////////////////////
// Name:        univ/listbox.cpp
// Purpose:     wxListBox implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     30.08.00
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
    #pragma implementation "univlistbox.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LISTBOX

#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/dcclient.h"
    #include "wx/listbox.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation of wxListBox
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)

BEGIN_EVENT_TABLE(wxListBox, wxListBoxBase)
    EVT_SIZE(wxListBox::OnSize)

    EVT_IDLE(wxListBox::OnIdle)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

void wxListBox::Init()
{
    // will be calculated later when needed
    m_lineHeight = 0;
    m_itemsPerPage = 0;

    // no items hence no current item
    m_current = -1;

    // no need to update anything initially
    m_updateCount = 0;
}

bool wxListBox::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint &pos,
                       const wxSize &size,
                       int n,
                       const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString &name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    SetBackgroundColour(*wxWHITE);

    if ( style & wxLB_SORT )
        m_strings = wxArrayString(TRUE /* auto sort */);

    Set(n, choices);

    SetBestSize(size);

    return TRUE;
}

wxListBox::~wxListBox()
{
}

// ----------------------------------------------------------------------------
// adding/inserting strings
// ----------------------------------------------------------------------------

int wxListBox::DoAppend(const wxString& item)
{
    size_t index = m_strings.Add(item);
    m_clientData.Insert(NULL, index);

    RefreshItem(m_strings.GetCount() - 1);

    return index;
}

void wxListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    // the position of the item being added to a sorted listbox can't be
    // specified
    wxCHECK_RET( !IsSorted(), _T("can't insert items into sorted listbox") );

    size_t count = items.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_strings.Insert(items[n], pos + n);
        m_clientData.Insert(NULL, pos + n);
    }

    RefreshItems(pos, count);
}

void wxListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    DoClear();

    size_t count = items.GetCount();
    m_strings.Alloc(count);
    m_clientData.Alloc(count);
    for ( size_t n = 0; n < count; n++ )
    {
        size_t index = m_strings.Add(items[n]);

        if ( clientData )
            m_clientData.Insert(clientData[n], index);
    }

    RefreshAll();
}

// ----------------------------------------------------------------------------
// removing strings
// ----------------------------------------------------------------------------

void wxListBox::DoClear()
{
    m_strings.Clear();

    if ( HasClientObjectData() )
    {
        size_t count = m_clientData.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            delete m_clientData[n];
        }
    }

    m_clientData.Clear();
}

void wxListBox::Clear()
{
    DoClear();

    RefreshAll();
}

void wxListBox::Delete(int n)
{
    wxCHECK_RET( n < GetCount(), _T("invalid index in wxListBox::Delete") );

    m_strings.RemoveAt(n);

    if ( HasClientObjectData() )
    {
        delete m_clientData[n];
    }

    m_clientData.RemoveAt(n);

    RefreshItems(n, GetCount() - n);
}

// ----------------------------------------------------------------------------
// client data handling
// ----------------------------------------------------------------------------

void wxListBox::DoSetItemClientData(int n, void* clientData)
{
    m_clientData[n] = clientData;
}

void *wxListBox::DoGetItemClientData(int n) const
{
    return m_clientData[n];
}

void wxListBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
    m_clientData[n] = clientData;
}

wxClientData* wxListBox::DoGetItemClientObject(int n) const
{
    return (wxClientData *)m_clientData[n];
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

void wxListBox::SetSelection(int n, bool select)
{
    if ( select )
    {
        if ( m_selections.Index(n) == wxNOT_FOUND )
        {
            m_selections.Add(n);

            RefreshItem(n);
        }
        //else: already selected
    }
    else // unselect
    {
        int index = m_selections.Index(n);
        if ( index != wxNOT_FOUND )
        {
            m_selections.RemoveAt(index);

            RefreshItem(n);
        }
        //else: not selected
    }
}

int wxListBox::GetSelection() const
{
    wxCHECK_MSG( !HasMultipleSelection(), -1,
                 _T("use wxListBox::GetSelections for ths listbox") );

    return m_selections.IsEmpty() ? -1 : m_selections[0];
}

int wxListBox::GetSelections(wxArrayInt& selections) const
{
    selections = m_selections;

    return m_selections.GetCount();
}

// ----------------------------------------------------------------------------
// refresh logic: we use delayed refreshing which allows to avoid multiple
// refreshes (and hence flicker) in case when several listbox items are
// added/deleted/changed subsequently
// ----------------------------------------------------------------------------

void wxListBox::RefreshItems(int from, int count)
{
    switch ( m_updateCount )
    {
        case 0:
            m_updateFrom = from;
            m_updateCount = count;
            break;

        case -1:
            // we refresh everything anyhow
            break;

        default:
            // add these items to the others which we have to refresh
            if ( m_updateFrom < from )
            {
                count += from - m_updateFrom;
                if ( m_updateCount < count )
                    m_updateCount = count;
            }
            else // m_updateFrom >= from
            {
                m_updateCount = wxMax(m_updateCount,
                                      from + count - m_updateFrom);
                m_updateFrom = from;
            }
    }
}

void wxListBox::RefreshItem(int n)
{
    switch ( m_updateCount )
    {
        case 0:
            // refresh this item only
            m_updateFrom = n;
            m_updateCount = 1;
            break;

        case -1:
            // we refresh everything anyhow
            break;

        default:
            // add this item to the others which we have to refresh
            if ( m_updateFrom < n )
            {
                if ( m_updateCount < n - m_updateFrom )
                    m_updateFrom = n - m_updateFrom;
            }
            else // n <= m_updateFrom
            {
                m_updateCount += m_updateFrom - n;
                m_updateFrom = n;
            }
    }
}

void wxListBox::RefreshAll()
{
    m_updateCount = -1;
}

void wxListBox::OnIdle(wxIdleEvent& event)
{
    if ( m_updateCount )
    {
        // only refresh the items which must be refreshed
        if ( m_updateCount == -1 )
        {
            // refresh all
            wxLogTrace(_T("listbox"), _T("Refreshing all"));

            Refresh();
        }
        else
        {
            wxLogTrace(_T("listbox"), _T("Refreshing items %d..%d"),
                       m_updateFrom, m_updateFrom + m_updateCount);

            wxRect rect;
            rect.x = 0;
            rect.y = m_updateFrom*GetLineHeight();
            rect.width = 32767; // larger than our size
            rect.height = m_updateCount*GetLineHeight();
            Refresh(TRUE, &rect);
        }

        m_updateCount = 0;
    }
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxListBox::DoDraw(wxControlRenderer *renderer)
{
    // draw the border first
    wxControl::DoDraw(renderer);

    // adjust the DC to account for scrolling
    wxDC& dc = renderer->GetDC();
    PrepareDC(dc);

    // get the items which must be redrawn
#if 0
    int y;
    GetViewStart(NULL, &y);
#endif
    wxCoord lineHeight = GetLineHeight();
    wxRect rectUpdate = GetUpdateRegion().GetBox();
    size_t itemFirst = rectUpdate.GetTop() / lineHeight,
           itemLast = (rectUpdate.GetBottom() + lineHeight - 1) / lineHeight,
           itemMax = m_strings.GetCount();

    if ( itemFirst >= itemMax )
        return;

    if ( itemLast > itemMax )
        itemLast = itemMax;

    // do draw them
    wxLogTrace(_T("listbox"), _T("Repainting items %d..%d"),
               itemFirst, itemLast);
    renderer->DrawItems(this, itemFirst, itemLast);
}

// ----------------------------------------------------------------------------
// size calculations
// ----------------------------------------------------------------------------

bool wxListBox::SetFont(const wxFont& font)
{
    if ( !wxControl::SetFont(font) )
        return FALSE;

    CalcItemsPerPage();

    RefreshAll();

    return TRUE;
}

void wxListBox::CalcItemsPerPage()
{
    m_lineHeight = wxClientDC(this).GetCharHeight();
    m_itemsPerPage = GetClientSize().y / m_lineHeight;
}

int wxListBox::GetItemsPerPage() const
{
    if ( !m_itemsPerPage )
    {
        wxConstCast(this, wxListBox)->CalcItemsPerPage();
    }

    return m_itemsPerPage;
}

wxCoord wxListBox::GetLineHeight() const
{
    if ( !m_lineHeight )
    {
        wxConstCast(this, wxListBox)->CalcItemsPerPage();
    }

    return m_lineHeight;
}

void wxListBox::OnSize(wxSizeEvent& event)
{
    // recalculate the number of items per page
    CalcItemsPerPage();

    event.Skip();
}

void wxListBox::DoSetFirstItem(int n)
{
    wxFAIL_MSG( _T("TODO") );
}

wxSize wxListBox::DoGetBestClientSize() const
{
    wxClientDC dc(wxConstCast(this, wxListBox));

    wxCoord width = 0,
            height = 0;

    size_t count = m_strings.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxCoord w,h;
        dc.GetTextExtent(m_strings[n], &w, &h);

        if ( w > width )
            width = w;
        if ( h > height )
            height = h;
    }

    // if the listbox is empty, still give it some non zero (even if
    // arbitrary) size - otherwise, leave small margin around the strings
    if ( !width )
        width = 100;
    else
        width += 3*dc.GetCharWidth();

    if ( !height )
        height = dc.GetCharHeight();

    // we need the height of the entire listbox, not just of one line
    height *= wxMax(count, 7);

    return wxSize(width, height);
}

// ----------------------------------------------------------------------------
// listbox actions
// ----------------------------------------------------------------------------

bool wxListBox::SendEvent(int item, wxEventType type)
{
    wxCommandEvent event(type, m_windowId);
    event.SetEventObject(this);

    int n;
    if ( m_selections.GetCount() > 0 )
    {
        n = m_selections[0];
        if ( HasClientObjectData() )
            event.SetClientObject(GetClientObject(n));
        else if ( HasClientUntypedData() )
            event.SetClientData(GetClientData(n));

        event.SetString(GetString(n));
    }
    else // no selection
    {
        n = -1;
    }

    event.m_commandInt = n;

    return GetEventHandler()->ProcessEvent(event);
}

void wxListBox::SetCurrentItem(int n)
{
    if ( m_current != -1 )
        RefreshItem(n);

    m_current = n;

    if ( m_current != -1 )
        RefreshItem(n);
}

void wxListBox::ChangeCurrent(int diff)
{
    int current = m_current == -1 ? 0 : m_current;

    current += diff;

    int last = GetCount() - 1;
    if ( current < 0 )
        current = 0;
    else if ( current > last )
        current = last;

    SetCurrentItem(current);
}

void wxListBox::Select(bool sel)
{
    if ( sel && !HasMultipleSelection() )
    {
        // deselect the old item first
        int selOld = GetSelection();
        if ( selOld != -1 )
        {
            SetSelection(selOld, FALSE);
        }
    }

    if ( m_current != -1 )
    {
        // [de]select the new one
        SetSelection(m_current, sel);

        if ( sel )
        {
            SendEvent(m_current, wxEVT_COMMAND_LISTBOX_SELECTED);
        }
    }
}

void wxListBox::Activate()
{
    if ( m_current != -1 )
    {
        Select();

        SendEvent(m_current, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED);
    }
}

bool wxListBox::PerformAction(const wxControlAction& action,
                              long numArg,
                              const wxString& strArg)
{
    if ( action == wxACTION_LISTBOX_SETFOCUS )
        SetCurrentItem(numArg);
    else if ( action == wxACTION_LISTBOX_ACTIVATE )
        Activate();
    else if ( action == wxACTION_LISTBOX_TOGGLE )
        Select(!IsSelected(m_current));
    else if ( action == wxACTION_LISTBOX_SELECT )
        Select(TRUE);
    else if ( action == wxACTION_LISTBOX_UNSELECT )
        Select(FALSE);
    else if ( action == wxACTION_LISTBOX_MOVEDOWN )
        ChangeCurrent(1);
    else if ( action == wxACTION_LISTBOX_MOVEUP )
        ChangeCurrent(-1);
    else if ( action == wxACTION_LISTBOX_PAGEDOWN )
        ChangeCurrent(GetItemsPerPage());
    else if ( action == wxACTION_LISTBOX_PAGEUP )
        ChangeCurrent(-GetItemsPerPage());
    else if ( action == wxACTION_LISTBOX_START )
        SetCurrentItem(0);
    else if ( action == wxACTION_LISTBOX_END )
        SetCurrentItem(GetCount() - 1);
    else
        return wxControl::PerformAction(action, numArg, strArg);

    return TRUE;
}

// ============================================================================
// implementation of wxStdListboxInputHandler
// ============================================================================

wxStdListboxInputHandler::wxStdListboxInputHandler(wxInputHandler *handler)
                        : wxStdInputHandler(handler)
{
}

bool wxStdListboxInputHandler::HandleKey(wxControl *control,
                                         const wxKeyEvent& event,
                                         bool pressed)
{
    // we're only interested in the key presses
    if ( pressed )
    {
        wxControlAction action;
        switch ( event.GetKeyCode() )
        {
            // movement
            case WXK_UP:    action = wxACTION_LISTBOX_MOVEUP; break;
            case WXK_DOWN:  action = wxACTION_LISTBOX_MOVEDOWN; break;
            case WXK_PRIOR: action = wxACTION_LISTBOX_PAGEUP; break;
            case WXK_NEXT:  action = wxACTION_LISTBOX_PAGEDOWN; break;
            case WXK_HOME:  action = wxACTION_LISTBOX_START; break;
            case WXK_END:   action = wxACTION_LISTBOX_END; break;

            // selection
            case WXK_SPACE: action = wxACTION_LISTBOX_TOGGLE; break;
            case WXK_RETURN:action = wxACTION_LISTBOX_ACTIVATE; break;
        }

        if ( !!action )
        {
            control->PerformAction(action);

            return TRUE;
        }
    }

    return wxStdInputHandler::HandleKey(control, event, pressed);
}

bool wxStdListboxInputHandler::HandleMouse(wxControl *control,
                                           const wxMouseEvent& event)
{
    if ( event.LeftDown() )
    {
        wxListBox *lbox = wxStaticCast(control, wxListBox);
        int item = event.GetPosition().y / lbox->GetLineHeight();
        if ( item < lbox->GetCount() )
        {
            lbox->PerformAction(wxACTION_LISTBOX_SETFOCUS, item);
            lbox->PerformAction(wxACTION_LISTBOX_TOGGLE);

            return TRUE;
        }
        //else: click outside the item area, ignore
    }

    return wxStdInputHandler::HandleMouse(control, event);
}

bool wxStdListboxInputHandler::HandleMouseMove(wxControl *control,
                                           const wxMouseEvent& event)
{
    return wxStdInputHandler::HandleMouseMove(control, event);
}

#endif // wxUSE_LISTBOX
