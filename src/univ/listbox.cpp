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
    m_maxWidth = 0;
    m_scrollRangeY = 0;

    // no items hence no current item
    m_current = -1;
    m_selAnchor = -1;
    m_currentChanged = FALSE;

    // no need to update anything initially
    m_updateCount = 0;

    // no scrollbars to show nor update
    m_updateScrollbarX =
    m_showScrollbarX =
    m_updateScrollbarY =
    m_showScrollbarY = FALSE;
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
    // for compatibility accept both the new and old styles - they mean the
    // same thing for us
    if ( style & wxLB_ALWAYS_SB )
        style |= wxALWAYS_SHOW_SB;

    // if we don't have neither multiple nor extended flag, we must have the
    // single selection listbox
    if ( !(style & (wxLB_MULTIPLE | wxLB_EXTENDED)) )
        style |= wxLB_SINGLE;

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    SetWindow(this);

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
    m_itemsClientData.Insert(NULL, index);

    m_updateScrollbarY = TRUE;

    if ( HasHorzScrollbar() )
    {
        // has the max width increased?
        wxCoord width;
        GetTextExtent(item, &width, NULL);
        if ( width > m_maxWidth )
        {
            m_maxWidth = width;
            m_updateScrollbarX = TRUE;
        }
    }

    RefreshFromItemToEnd(index);

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
        m_itemsClientData.Insert(NULL, pos + n);
    }

    // the number of items has changed so we might have to show the scrollbar
    m_updateScrollbarY = TRUE;

    // the max width also might have changed - just recalculate it instead of
    // keeping track of it here, this is probably more efficient for a typical
    // use pattern
    m_maxWidth = 0;
    m_updateScrollbarX = TRUE;

    // note that we have to refresh all the items after the ones we inserted,
    // not just these items
    RefreshFromItemToEnd(pos);
}

void wxListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    DoClear();

    size_t count = items.GetCount();
    if ( !count )
        return;

    m_strings.Alloc(count);
    m_itemsClientData.Alloc(count);
    for ( size_t n = 0; n < count; n++ )
    {
        size_t index = m_strings.Add(items[n]);
        m_itemsClientData.Insert(clientData ? clientData[n] : NULL, index);
    }

    m_updateScrollbarY = TRUE;

    RefreshAll();
}

void wxListBox::SetString(int n, const wxString& s)
{
    if ( HasHorzScrollbar() )
    {
        // we need to update m_maxWidth as changing the string may cause the
        // horz scrollbar [dis]appear
        wxCoord widthOld;
        GetTextExtent(m_strings[n], &widthOld, NULL);
        m_strings[n] = s;

        // has the max length changed?
        wxCoord width;
        GetTextExtent(s, &width, NULL);

        // it might have increased if the new string is long
        if ( width > m_maxWidth )
        {
            m_maxWidth = width;
            m_updateScrollbarX = TRUE;
        }
        // or also decreased if the old string was the longest one
        else if ( (width < m_maxWidth) && (widthOld == m_maxWidth) )
        {
            m_maxWidth = 0;
            m_updateScrollbarX = TRUE;
        }
    }
    else // no horz scrollbar
    {
        m_strings[n] = s;
    }

    RefreshItem(n);
}

// ----------------------------------------------------------------------------
// removing strings
// ----------------------------------------------------------------------------

void wxListBox::DoClear()
{
    m_strings.Clear();

    if ( HasClientObjectData() )
    {
        size_t count = m_itemsClientData.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            delete m_itemsClientData[n];
        }
    }

    m_itemsClientData.Clear();
}

void wxListBox::Clear()
{
    DoClear();

    m_updateScrollbarY = TRUE;

    RefreshAll();
}

void wxListBox::Delete(int n)
{
    wxCHECK_RET( n < GetCount(), _T("invalid index in wxListBox::Delete") );

    // do it before removing the index as otherwise the last item will not be
    // refreshed (as GetCount() will be decremented)
    RefreshFromItemToEnd(n);

    m_strings.RemoveAt(n);

    if ( HasClientObjectData() )
    {
        delete m_itemsClientData[n];
    }

    m_itemsClientData.RemoveAt(n);

    m_updateScrollbarY = TRUE;
}

// ----------------------------------------------------------------------------
// client data handling
// ----------------------------------------------------------------------------

void wxListBox::DoSetItemClientData(int n, void* clientData)
{
    m_itemsClientData[n] = clientData;
}

void *wxListBox::DoGetItemClientData(int n) const
{
    return m_itemsClientData[n];
}

void wxListBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
    m_itemsClientData[n] = clientData;
}

wxClientData* wxListBox::DoGetItemClientObject(int n) const
{
    return (wxClientData *)m_itemsClientData[n];
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

void wxListBox::Refresh(bool eraseBackground, const wxRect *rect)
{
    if ( rect )
        wxLogTrace(_T("listbox"), _T("Refreshing (%d, %d)-(%d, %d)"),
                   rect->x, rect->y,
                   rect->x + rect->width, rect->y + rect->height);
    else
        wxLogTrace(_T("listbox"), _T("Refreshing all"));

    wxControl::Refresh(eraseBackground, rect);
}

void wxListBox::RefreshFromItemToEnd(int from)
{
    RefreshItems(from, GetCount() - from);
}

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
                if ( m_updateCount < n - m_updateFrom + 1 )
                    m_updateCount = n - m_updateFrom + 1;
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

void wxListBox::UpdateScrollbars()
{
    wxSize size = GetClientSize();

    // is our height enough to show all items?
    int nLines = GetCount();
    wxCoord lineHeight = GetLineHeight();
    bool showScrollbarY = nLines*lineHeight > size.y;

    // check the width too if required
    wxCoord charWidth, maxWidth;
    bool showScrollbarX;
    if ( HasHorzScrollbar() )
    {
        charWidth = GetCharWidth();
        maxWidth = GetMaxWidth();
        showScrollbarX = maxWidth > size.x;
    }
    else // never show it
    {
        charWidth = maxWidth = 0;
        showScrollbarX = FALSE;
    }

    // what should be the scrollbar range now?
    int scrollRangeX = showScrollbarX
                        ? (maxWidth + 2*charWidth - 1) / charWidth
                        : 0;
    int scrollRangeY = showScrollbarY ? nLines : 0;

    // reset scrollbars if something changed: either the visibility status
    // or the range of a scrollbar which is shown
    if ( (showScrollbarY != m_showScrollbarY) ||
         (showScrollbarX != m_showScrollbarX) ||
         (showScrollbarY && (scrollRangeY != m_scrollRangeY)) ||
         (showScrollbarX && (scrollRangeX != m_scrollRangeX)) )
    {
        int x, y;
        GetViewStart(&x, &y);
        SetScrollbars(charWidth, lineHeight,
                      scrollRangeX, scrollRangeY,
                      x, y);

        m_showScrollbarX = showScrollbarX;
        m_showScrollbarY = showScrollbarY;

        m_scrollRangeX = scrollRangeX;
        m_scrollRangeY = scrollRangeY;
    }
}

void wxListBox::UpdateItems()
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
        wxSize size = GetClientSize();
        wxRect rect;
        rect.width = size.x;
        rect.height = size.y;
        rect.y += m_updateFrom*GetLineHeight();
        rect.height = m_updateCount*GetLineHeight();
        CalcScrolledPosition(rect.x, rect.y, &rect.x, &rect.y);

        wxLogTrace(_T("listbox"), _T("Refreshing items %d..%d (%d-%d)"),
                   m_updateFrom, m_updateFrom + m_updateCount - 1,
                   rect.GetTop(), rect.GetBottom());

        Refresh(TRUE, &rect);
    }
}

void wxListBox::OnIdle(wxIdleEvent& event)
{
    if ( m_updateScrollbarY || m_updateScrollbarX )
    {
        UpdateScrollbars();

        m_updateScrollbarX = FALSE;
        m_updateScrollbarY = FALSE;
    }

    if ( m_currentChanged )
    {
        EnsureVisible();

        m_currentChanged = FALSE;
    }

    if ( m_updateCount )
    {
        UpdateItems();

        m_updateCount = 0;
    }
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

wxBorder wxListBox::GetDefaultBorder() const
{
    return wxBORDER_SUNKEN;
}

void wxListBox::DoDraw(wxControlRenderer *renderer)
{
    // adjust the DC to account for scrolling
    wxDC& dc = renderer->GetDC();
    PrepareDC(dc);
    dc.SetFont(GetFont());

    // get the update rect
    wxRegion rgnUpdate = GetUpdateRegion();
    rgnUpdate.Intersect(GetClientRect());
    wxRect rectUpdate = rgnUpdate.GetBox();
    wxPoint ptOrigin = GetClientAreaOrigin();
    rectUpdate.x -= ptOrigin.x;
    rectUpdate.y -= ptOrigin.y;

    int yTop, yBottom;
    CalcUnscrolledPosition(0, rectUpdate.GetTop(), NULL, &yTop);
    CalcUnscrolledPosition(0, rectUpdate.GetBottom(), NULL, &yBottom);

    // get the items which must be redrawn
    wxCoord lineHeight = GetLineHeight();
    size_t itemFirst = yTop / lineHeight,
           itemLast = (yBottom + lineHeight - 1) / lineHeight,
           itemMax = m_strings.GetCount();

    if ( itemFirst >= itemMax )
        return;

    if ( itemLast > itemMax )
        itemLast = itemMax;

    // do draw them
    wxLogTrace(_T("listbox"), _T("Repainting items %d..%d"),
               itemFirst, itemLast);

    DoDrawRange(renderer, itemFirst, itemLast);
}

void wxListBox::DoDrawRange(wxControlRenderer *renderer,
                            int itemFirst, int itemLast)
{
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
    m_lineHeight = GetRenderer()->GetListboxItemHeight(GetCharHeight());
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

wxCoord wxListBox::GetMaxWidth() const
{
    if ( m_maxWidth == 0 )
    {
        wxCoord width;
        size_t count = m_strings.GetCount();
        for ( size_t n =0; n < count; n++ )
        {
            GetTextExtent(m_strings[n], &width, NULL);
            if ( width > m_maxWidth )
            {
                wxConstCast(this, wxListBox)->m_maxWidth = width;
            }
        }
    }

    return m_maxWidth;
}

void wxListBox::OnSize(wxSizeEvent& event)
{
    // recalculate the number of items per page
    CalcItemsPerPage();

    event.Skip();
}

void wxListBox::DoSetFirstItem(int n)
{
    SetCurrentItem(n);
}

wxSize wxListBox::DoGetBestClientSize() const
{
    wxCoord width = 0,
            height = 0;

    size_t count = m_strings.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxCoord w,h;
        GetTextExtent(m_strings[n], &w, &h);

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
        width += 3*GetCharWidth();

    if ( !height )
        height = GetCharHeight();

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

    if ( item != -1 )
    {
        if ( HasClientObjectData() )
            event.SetClientObject(GetClientObject(item));
        else if ( HasClientUntypedData() )
            event.SetClientData(GetClientData(item));

        event.SetString(GetString(item));
    }

    event.m_commandInt = item;

    return GetEventHandler()->ProcessEvent(event);
}

void wxListBox::SetCurrentItem(int n)
{
    if ( n != m_current )
    {
        if ( m_current != -1 )
            RefreshItem(m_current);

        m_current = n;

        if ( m_current != -1 )
        {
            m_currentChanged = TRUE;

            RefreshItem(m_current);
        }
    }
    //else: nothing to do
}

bool wxListBox::FindItem(const wxString& prefix)
{
    size_t len = prefix.length();
    int count = GetCount();
    for ( int item = m_current + 1; item != m_current; item++ )
    {
        if ( item == count )
        {
            // wrap
            item = 0;

            if ( m_current == -1 )
                break;
        }

        if ( wxStrnicmp(m_strings[item], prefix, len) == 0 )
        {
            SetCurrentItem(item);

            if ( !(GetWindowStyle() & wxLB_MULTIPLE) )
            {
                DeselectAll(item);
                Select(TRUE, item);

                if ( GetWindowStyle() & wxLB_EXTENDED )
                    AnchorSelection(item);
            }

            return TRUE;
        }
    }

    // nothing found
    return FALSE;
}

void wxListBox::EnsureVisible()
{
    if ( !m_showScrollbarY )
    {
        // nothing to do - everything is shown anyhow
        return;
    }

    int first;
    GetViewStart(0, &first);
    if ( first > m_current )
    {
        // we need to scroll upwards, so make the current item appear on top
        // of the shown range
        Scroll(0, m_current);
    }
    else
    {
        int last = first + GetClientSize().y / GetLineHeight() - 1;
        if ( last < m_current )
        {
            // scroll down: the current item appears at the bottom of the
            // range
            Scroll(0, m_current - (last - first));
        }
    }
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

void wxListBox::ExtendSelection(int itemTo)
{
    // if we don't have the explicit values for selection start/end, make them
    // up
    if ( m_selAnchor == -1 )
        m_selAnchor = m_current;

    if ( itemTo == -1 )
        itemTo = m_current;

    // swap the start/end of selection range if necessary
    int itemFrom = m_selAnchor;
    if ( itemFrom > itemTo )
    {
        int itemTmp = itemFrom;
        itemFrom = itemTo;
        itemTo = itemTmp;
    }

    // the selection should now include all items in the range between the
    // anchor and the specified item and only them

    int n;
    for ( n = 0; n < itemFrom; n++ )
    {
        Deselect(n);
    }

    for ( ; n <= itemTo; n++ )
    {
        SetSelection(n);
    }

    int count = GetCount();
    for ( ; n < count; n++ )
    {
        Deselect(n);
    }
}

void wxListBox::Select(bool sel, int item)
{
    if ( item != -1 )
        SetCurrentItem(item);

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

void wxListBox::Activate(int item)
{
    if ( item != -1 )
        SetCurrentItem(item);

    if ( m_current != -1 )
    {
        Select();

        SendEvent(m_current, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED);
    }
}

// ----------------------------------------------------------------------------
// input handling
// ----------------------------------------------------------------------------

wxString wxListBox::GetInputHandlerType() const
{
    return wxINP_HANDLER_LISTBOX;
}

bool wxListBox::PerformAction(const wxControlAction& action,
                              long numArg,
                              const wxString& strArg)
{
    int item = (int)numArg;

    if ( action == wxACTION_LISTBOX_SETFOCUS )
        SetCurrentItem(item);
    else if ( action == wxACTION_LISTBOX_ACTIVATE )
        Activate(item);
    else if ( action == wxACTION_LISTBOX_TOGGLE )
    {
        if ( item == -1 )
            item = m_current;
        Select(!IsSelected(item), item);
    }
    else if ( action == wxACTION_LISTBOX_SELECT )
    {
        DeselectAll(item);
        Select(TRUE, item);
    }
    else if ( action == wxACTION_LISTBOX_SELECTADD )
        Select(TRUE, item);
    else if ( action == wxACTION_LISTBOX_UNSELECT )
        Select(FALSE, item);
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
    else if ( action == wxACTION_LISTBOX_UNSELECTALL )
        DeselectAll(item);
    else if ( action == wxACTION_LISTBOX_EXTENDSEL )
        ExtendSelection(item);
    else if ( action == wxACTION_LISTBOX_FIND )
        FindItem(strArg);
    else if ( action == wxACTION_LISTBOX_ANCHOR )
        AnchorSelection(item == -1 ? m_current : item);
    else if ( action == wxACTION_LISTBOX_SELECTALL ||
                action == wxACTION_LISTBOX_SELTOGGLE )
        wxFAIL_MSG(_T("unimplemented yet"));
    else
        return wxControl::PerformAction(action, numArg, strArg);

    return TRUE;
}

// ============================================================================
// implementation of wxStdListboxInputHandler
// ============================================================================

wxStdListboxInputHandler::wxStdListboxInputHandler(wxInputHandler *handler,
                                                   bool toggleOnPressAlways)
                        : wxStdInputHandler(handler)
{
    m_winCapture = NULL;
    m_btnCapture = 0;
    m_toggleOnPressAlways = toggleOnPressAlways;
}

int wxStdListboxInputHandler::HitTest(const wxListBox *lbox,
                                      const wxMouseEvent& event)
{
    wxPoint pt = event.GetPosition();
    pt -= lbox->GetClientAreaOrigin();
    int y;
    lbox->CalcUnscrolledPosition(0, pt.y, NULL, &y);
    int item = y / lbox->GetLineHeight();

    return (item >= 0) && (item < lbox->GetCount()) ? item : -1;
}

bool wxStdListboxInputHandler::HandleKey(wxControl *control,
                                         const wxKeyEvent& event,
                                         bool pressed)
{
    // we're only interested in the key press events
    if ( pressed && !event.AltDown() )
    {
        bool isMoveCmd = TRUE;
        int style = control->GetWindowStyle();

        wxControlAction action;
        wxString strArg;

        int keycode = event.GetKeyCode();
        switch ( keycode )
        {
            // movement
            case WXK_UP:    action = wxACTION_LISTBOX_MOVEUP; break;
            case WXK_DOWN:  action = wxACTION_LISTBOX_MOVEDOWN; break;
            case WXK_PRIOR: action = wxACTION_LISTBOX_PAGEUP; break;
            case WXK_NEXT:  action = wxACTION_LISTBOX_PAGEDOWN; break;
            case WXK_HOME:  action = wxACTION_LISTBOX_START; break;
            case WXK_END:   action = wxACTION_LISTBOX_END; break;

            // selection
            case WXK_SPACE:
                if ( style & wxLB_MULTIPLE )
                {
                    action = wxACTION_LISTBOX_TOGGLE;
                    isMoveCmd = FALSE;
                }
                break;

            case WXK_RETURN:
                action = wxACTION_LISTBOX_ACTIVATE;
                isMoveCmd = FALSE;
                break;

            default:
                if ( (keycode < 255) && wxIsalnum(keycode) )
                {
                    action = wxACTION_LISTBOX_FIND;
                    strArg = (wxChar)keycode;
                }
        }

        if ( !!action )
        {
            control->PerformAction(action, -1, strArg);

            if ( isMoveCmd )
            {
                if ( style & wxLB_SINGLE )
                {
                    // the current item is always the one selected
                    control->PerformAction(wxACTION_LISTBOX_SELECT);
                }
                else if ( style & wxLB_EXTENDED )
                {
                    if ( event.ShiftDown() )
                        control->PerformAction(wxACTION_LISTBOX_EXTENDSEL);
                    else
                    {
                        // select the item and make it the new selection anchor
                        control->PerformAction(wxACTION_LISTBOX_SELECT);
                        control->PerformAction(wxACTION_LISTBOX_ANCHOR);
                    }
                }
                //else: nothing to do for multiple selection listboxes
            }

            return TRUE;
        }
    }

    return wxStdInputHandler::HandleKey(control, event, pressed);
}

bool wxStdListboxInputHandler::HandleMouse(wxControl *control,
                                           const wxMouseEvent& event)
{
    // single and extended listboxes behave similarly with respect to the
    // mouse events: for both of them clicking the item selects or toggles it,
    // but multiple selection listboxes are different: the item is focused
    // when clicked and only toggled when the button is released
    if ( (m_btnCapture &&
          (control->GetWindowStyle() & wxLB_MULTIPLE) &&
          event.ButtonUp(m_btnCapture))
            || event.ButtonDown()
            || event.LeftDClick() )
    {
        wxListBox *lbox = wxStaticCast(control, wxListBox);
        int item = HitTest(lbox, event);
        if ( item != -1 )
        {
            wxControlAction action;
            if ( m_btnCapture && event.ButtonUp(m_btnCapture) )
            {
                if ( m_winCapture )
                {
                    m_winCapture->ReleaseMouse();
                    m_winCapture = NULL;
                    m_btnCapture = 0;
                }
                else
                {
                    // this is not supposed to happen - if we get here, the
                    // even is from the mouse button which had been pressed
                    // before and we must have captured the mouse back then
                    wxFAIL_MSG(_T("logic error in listbox mosue handling"));
                }

                if ( !m_toggleOnPressAlways )
                {
                    // in this mode we toggle the item state when the button is
                    // released, i.e. now
                    action = wxACTION_LISTBOX_TOGGLE;
                }
            }
            else if ( event.ButtonDown() )
            {
                if ( lbox->HasMultipleSelection() )
                {
                    if ( lbox->GetWindowStyle() & wxLB_MULTIPLE )
                    {
                        // capture the mouse to track the selected item
                        m_winCapture = lbox;
                        m_winCapture->CaptureMouse();
                        m_btnCapture = event.LeftDown()
                                        ? 1
                                        : event.RightDown()
                                            ? 3
                                            : 2;

                        if ( m_toggleOnPressAlways )
                        {
                            // toggle the item right now
                            action = wxACTION_LISTBOX_TOGGLE;
                        }
                    }
                    else // wxLB_EXTENDED listbox
                    {
                        // simple click in an extended listbox clears the
                        // selection, ctrl-click toggles an item to it and
                        // shift-click adds a range
                        if ( event.ControlDown() )
                        {
                            control->PerformAction(wxACTION_LISTBOX_ANCHOR,
                                                   item);

                            action = wxACTION_LISTBOX_TOGGLE;
                        }
                        else if ( event.ShiftDown() )
                        {
                            action = wxACTION_LISTBOX_EXTENDSEL;
                        }
                        else // simple click
                        {
                            control->PerformAction(wxACTION_LISTBOX_ANCHOR,
                                                   item);

                            action = wxACTION_LISTBOX_SELECT;
                        }
                    }
                }
                else // single selection
                {
                    action = wxACTION_LISTBOX_SELECT;
                }

                // in any case, clicking an item makes it the current one
                lbox->PerformAction(wxACTION_LISTBOX_SETFOCUS, item);
            }
            else // event.LeftDClick()
            {
                action = wxACTION_LISTBOX_ACTIVATE;
            }

            if ( !!action )
            {
                lbox->PerformAction(action, item);
            }

            return TRUE;
        }
        //else: click outside the item area, ignore
    }

    return wxStdInputHandler::HandleMouse(control, event);
}

bool wxStdListboxInputHandler::HandleMouseMove(wxControl *control,
                                           const wxMouseEvent& event)
{
    if ( !m_winCapture || (event.GetEventObject() != m_winCapture) )
    {
        // we don't react to this
        return FALSE;
    }

    // TODO: not yet... should track the mouse outside and start an auto
    //       scroll timer - but this should be probably done in
    //       wxScrolledWindow itself (?)
    if ( !event.Moving() )
        return FALSE;

    wxListBox *lbox = wxStaticCast(control, wxListBox);
    int item = HitTest(lbox, event);
    if ( item == -1 )
    {
        // mouse is below the last item
        return FALSE;
    }

    lbox->PerformAction(wxACTION_LISTBOX_SETFOCUS, item);

    return TRUE;
}

#endif // wxUSE_LISTBOX
