///////////////////////////////////////////////////////////////////////////////
// Name:        listbox.cpp
// Purpose:     wxListBox
// Author:      David Webster
// Modified by:
// Created:     10/09/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/window.h"
#include "wx/os2/private.h"

#ifndef WX_PRECOMP
#include "wx/listbox.h"
#include "wx/settings.h"
#include "wx/brush.h"
#include "wx/font.h"
#include "wx/dc.h"
#include "wx/utils.h"
#endif

#define INCL_M
#include <os2.h>

#include "wx/dynarray.h"
#include "wx/log.h"

#if wxUSE_OWNER_DRAWN
    #include  "wx/ownerdrw.h"
#endif

#if !USE_SHARED_LIBRARY
  IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)
#endif

// ============================================================================
// list box item declaration and implementation
// ============================================================================

#if wxUSE_OWNER_DRAWN

class wxListBoxItem : public wxOwnerDrawn
{
public:
    wxListBoxItem(const wxString& str = "");
};

wxListBoxItem::wxListBoxItem(const wxString& str) : wxOwnerDrawn(str, FALSE)
{
    // no bitmaps/checkmarks
    SetMarginWidth(0);
}

wxOwnerDrawn *wxListBox::CreateItem(size_t n)
{
    return new wxListBoxItem();
}

#endif  //USE_OWNER_DRAWN

// ============================================================================
// list box control implementation
// ============================================================================

// Listbox item
wxListBox::wxListBox()
{
    m_noItems = 0;
    m_selected = 0;
}

bool wxListBox::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n, const wxString choices[],
                       long style,
#if wxUSE_VALIDATORS
                       const wxValidator& validator,
#endif
                       const wxString& name)
{
    m_noItems = 0;
    m_hWnd = 0;
    m_selected = 0;

    SetName(name);
#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    if (parent)
        parent->AddChild(this);

    wxSystemSettings settings;
    SetBackgroundColour(settings.GetSystemColour(wxSYS_COLOUR_WINDOW));
    SetForegroundColour(parent->GetForegroundColour());

    m_windowId = ( id == -1 ) ? (int)NewControlId() : id;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;
    m_windowStyle = style;

// TODO:
/*
    DWORD wstyle = WS_VISIBLE | WS_VSCROLL | WS_TABSTOP |
                   LBS_NOTIFY | LBS_HASSTRINGS;
    if (m_windowStyle & wxLB_MULTIPLE)
        wstyle |= LBS_MULTIPLESEL;
    else if (m_windowStyle & wxLB_EXTENDED)
        wstyle |= LBS_EXTENDEDSEL;

    if (m_windowStyle & wxLB_ALWAYS_SB)
        wstyle |= LBS_DISABLENOSCROLL;
    if (m_windowStyle & wxLB_HSCROLL)
        wstyle |= WS_HSCROLL;
    if (m_windowStyle & wxLB_SORT)
        wstyle |= LBS_SORT;

#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW ) {
        // we don't support LBS_OWNERDRAWVARIABLE yet
        wstyle |= LBS_OWNERDRAWFIXED;
    }
#endif

    // Without this style, you get unexpected heights, so e.g. constraint layout
    // doesn't work properly
    wstyle |= LBS_NOINTEGRALHEIGHT;

    bool want3D;
    WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D);

    // Even with extended styles, need to combine with WS_BORDER for them to
    // look right.
    if ( want3D || wxStyleHasBorder(m_windowStyle) )
    {
        wstyle |= WS_BORDER;
    }

    m_hWnd = (WXHWND)::CreateWindowEx(exStyle, wxT("LISTBOX"), NULL,
            wstyle | WS_CHILD,
            0, 0, 0, 0,
            (HWND)parent->GetHWND(), (HMENU)m_windowId,
            wxGetInstance(), NULL);

    wxCHECK_MSG( m_hWnd, FALSE, wxT("Failed to create listbox") );

    // Subclass again to catch messages
    SubclassWin(m_hWnd);

    size_t ui;
    for (ui = 0; ui < (size_t)n; ui++) {
        Append(choices[ui]);
    }

    if ( (m_windowStyle & wxLB_MULTIPLE) == 0 )
        SendMessage(GetHwnd(), LB_SETCURSEL, 0, 0);
*/
    SetFont(parent->GetFont());

    SetSize(x, y, width, height);

    Show(TRUE);

    return TRUE;
}

wxListBox::~wxListBox()
{
#if wxUSE_OWNER_DRAWN
    size_t uiCount = m_aItems.Count();
    while ( uiCount-- != 0 ) {
        delete m_aItems[uiCount];
    }
#endif // wxUSE_OWNER_DRAWN
}

void wxListBox::SetupColours()
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW));
    SetForegroundColour(GetParent()->GetForegroundColour());
}

// ----------------------------------------------------------------------------
// implementation of wxListBoxBase methods
// ----------------------------------------------------------------------------

void wxListBox::DoSetFirstItem(int N)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 wxT("invalid index in wxListBox::SetFirstItem") );

//    SendMessage(GetHwnd(), LB_SETTOPINDEX, (WPARAM)N, (LPARAM)0);
}

void wxListBox::Delete(int N)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 wxT("invalid index in wxListBox::Delete") );

#if wxUSE_OWNER_DRAWN
    delete m_aItems[N];
    m_aItems.Remove(N);
#else // !wxUSE_OWNER_DRAWN
    if ( HasClientObjectData() )
    {
        delete GetClientObject(N);
    }
#endif // wxUSE_OWNER_DRAWN/!wxUSE_OWNER_DRAWN

//    SendMessage(GetHwnd(), LB_DELETESTRING, N, 0);
    m_noItems--;

    SetHorizontalExtent("");
}

int wxListBox::DoAppend(const wxString& item)
{
// TODO:
/*
    int index = ListBox_AddString(GetHwnd(), item);
    m_noItems++;

#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW ) {
        wxOwnerDrawn *pNewItem = CreateItem(index); // dummy argument
        pNewItem->SetName(item);
        m_aItems.Add(pNewItem);
        ListBox_SetItemData(GetHwnd(), index, pNewItem);
    }
#endif

    SetHorizontalExtent(item);

    return index;
*/
    return 0;
}

void wxListBox::DoSetItems(const wxArrayString& choices, void** clientData)
{
// TODO:
/*
    ShowWindow(GetHwnd(), SW_HIDE);

    ListBox_ResetContent(GetHwnd());

    m_noItems = choices.GetCount();
    int i;
    for (i = 0; i < m_noItems; i++)
    {
        ListBox_AddString(GetHwnd(), choices[i]);
        if ( clientData )
        {
#if wxUSE_OWNER_DRAWN
            wxASSERT_MSG(clientData[ui] == NULL,
                         wxT("Can't use client data with owner-drawn listboxes"));
#else // !wxUSE_OWNER_DRAWN
            ListBox_SetItemData(GetHwnd(), i, clientData[i]);
#endif // wxUSE_OWNER_DRAWN/!wxUSE_OWNER_DRAWN
        }
    }

#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW ) {
        // first delete old items
        size_t ui = m_aItems.Count();
        while ( ui-- != 0 ) {
            delete m_aItems[ui];
        }
        m_aItems.Empty();

        // then create new ones
        for ( ui = 0; ui < (size_t)m_noItems; ui++ ) {
            wxOwnerDrawn *pNewItem = CreateItem(ui);
            pNewItem->SetName(choices[ui]);
            m_aItems.Add(pNewItem);
            ListBox_SetItemData(GetHwnd(), ui, pNewItem);
        }
    }
#endif // wxUSE_OWNER_DRAWN

    SetHorizontalExtent();

    ShowWindow(GetHwnd(), SW_SHOW);
*/
}

int wxListBox::FindString(const wxString& s) const
{
// TODO:
/*
    int pos = ListBox_FindStringExact(GetHwnd(), (WPARAM)-1, s);
    if (pos == LB_ERR)
        return wxNOT_FOUND;
    else
        return pos;
*/
    return 0;
}

void wxListBox::Clear()
{
#if wxUSE_OWNER_DRAWN
    size_t uiCount = m_aItems.Count();
    while ( uiCount-- != 0 ) {
        delete m_aItems[uiCount];
    }

    m_aItems.Clear();
#else // !wxUSE_OWNER_DRAWN
    if ( HasClientObjectData() )
    {
        for ( size_t n = 0; n < (size_t)m_noItems; n++ )
        {
            delete GetClientObject(n);
        }
    }
#endif // wxUSE_OWNER_DRAWN/!wxUSE_OWNER_DRAWN
// TODO:
/*

    ListBox_ResetContent(GetHwnd());

    m_noItems = 0;
    SetHorizontalExtent();
*/
}

void wxListBox::SetSelection(int N, bool select)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 wxT("invalid index in wxListBox::SetSelection") );
// TODO:
/*

    if ( HasMultipleSelection() )
    {
        SendMessage(GetHwnd(), LB_SETSEL, select, N);
    }
    else
    {
        SendMessage(GetHwnd(), LB_SETCURSEL, select ? N : -1, 0);
    }
*/
}

bool wxListBox::IsSelected(int N) const
{
    wxCHECK_MSG( N >= 0 && N < m_noItems, FALSE,
                 wxT("invalid index in wxListBox::Selected") );

//    return SendMessage(GetHwnd(), LB_GETSEL, N, 0) == 0 ? FALSE : TRUE;
    return FALSE;
}

wxClientData* wxListBox::DoGetItemClientObject(int n) const
{
    return (wxClientData *)DoGetItemClientData(n);
}

void *wxListBox::DoGetItemClientData(int n) const
{
    wxCHECK_MSG( n >= 0 && n < m_noItems, NULL,
                 wxT("invalid index in wxListBox::GetClientData") );

//    return (void *)SendMessage(GetHwnd(), LB_GETITEMDATA, n, 0);
    return NULL;
}

void wxListBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
    DoSetItemClientData(n, clientData);
}

void wxListBox::DoSetItemClientData(int n, void *clientData)
{
    wxCHECK_RET( n >= 0 && n < m_noItems,
                 wxT("invalid index in wxListBox::SetClientData") );

#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        // client data must be pointer to wxOwnerDrawn, otherwise we would crash
        // in OnMeasure/OnDraw.
        wxFAIL_MSG(wxT("Can't use client data with owner-drawn listboxes"));
    }
#endif // wxUSE_OWNER_DRAWN

// TODO:
/*
    if ( ListBox_SetItemData(GetHwnd(), n, clientData) == LB_ERR )
        wxLogDebug(wxT("LB_SETITEMDATA failed"));
*/
}

bool wxListBox::HasMultipleSelection() const
{
    return (m_windowStyle & wxLB_MULTIPLE) || (m_windowStyle & wxLB_EXTENDED);
}

// Return number of selections and an array of selected integers
int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.Empty();

// TODO:
/*
    if ( HasMultipleSelection() )
    {
        int no_sel = ListBox_GetSelCount(GetHwnd());
        if (no_sel != 0) {
            int *selections = new int[no_sel];
            int rc = ListBox_GetSelItems(GetHwnd(), no_sel, selections);

            wxCHECK_MSG(rc != LB_ERR, -1, wxT("ListBox_GetSelItems failed"));

            aSelections.Alloc(no_sel);
            for ( int n = 0; n < no_sel; n++ )
                aSelections.Add(selections[n]);

            delete [] selections;
        }

        return no_sel;
    }
    else  // single-selection listbox
    {
        aSelections.Add(ListBox_GetCurSel(GetHwnd()));

        return 1;
    }
*/
    return 0;
}

// Get single selection, for single choice list items
int wxListBox::GetSelection() const
{
    wxCHECK_MSG( !HasMultipleSelection(),
                 -1,
                 wxT("GetSelection() can't be used with multiple-selection "
                    "listboxes, use GetSelections() instead.") );

//    return ListBox_GetCurSel(GetHwnd());
    return 0;
}

// Find string for position
wxString wxListBox::GetString(int N) const
{
    wxCHECK_MSG( N >= 0 && N < m_noItems, "",
                 wxT("invalid index in wxListBox::GetClientData") );

// TODO:
/*
    int len = ListBox_GetTextLen(GetHwnd(), N);

    // +1 for terminating NUL
    wxString result;
    ListBox_GetText(GetHwnd(), N, result.GetWriteBuf(len + 1));
    result.UngetWriteBuf();

    return result;
*/
    return((wxString)"");
}

void
wxListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    wxCHECK_RET( pos >= 0 && pos <= m_noItems,
                 wxT("invalid index in wxListBox::InsertItems") );

// TODO:
/*
    int nItems = items.GetCount();
    for ( int i = 0; i < nItems; i++ )
        ListBox_InsertString(GetHwnd(), i + pos, items[i]);
    m_noItems += nItems;

    SetHorizontalExtent();
*/
}

void wxListBox::SetString(int N, const wxString& s)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 wxT("invalid index in wxListBox::SetString") );

    // remember the state of the item
    bool wasSelected = IsSelected(N);

    void *oldData = NULL;
    wxClientData *oldObjData = NULL;
    if ( m_clientDataItemsType == ClientData_Void )
        oldData = GetClientData(N);
    else if ( m_clientDataItemsType == ClientData_Object )
        oldObjData = GetClientObject(N);
// TODO:
/*

    // delete and recreate it
    SendMessage(GetHwnd(), LB_DELETESTRING, N, 0);

    int newN = N;
    if ( N == m_noItems - 1 )
        newN = -1;

    ListBox_InsertString(GetHwnd(), newN, s);

    // restore the client data
    if ( oldData )
        SetClientData(N, oldData);
    else if ( oldObjData )
        SetClientObject(N, oldObjData);

    // we may have lost the selection
    if ( wasSelected )
        Select(N);

#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
        // update item's text
        m_aItems[N]->SetName(s);
#endif  //USE_OWNER_DRAWN
*/
}

int wxListBox::GetCount() const
{
    return m_noItems;
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

// Windows-specific code to set the horizontal extent of the listbox, if
// necessary. If s is non-NULL, it's used to calculate the horizontal extent.
// Otherwise, all strings are used.
void wxListBox::SetHorizontalExtent(const wxString& s)
{
// TODO:
/*
    // Only necessary if we want a horizontal scrollbar
    if (!(m_windowStyle & wxHSCROLL))
        return;
    TEXTMETRIC lpTextMetric;

    if ( !s.IsEmpty() )
    {
        int existingExtent = (int)SendMessage(GetHwnd(), LB_GETHORIZONTALEXTENT, 0, 0L);
        HDC dc = GetWindowDC(GetHwnd());
        HFONT oldFont = 0;
        if (GetFont().Ok() && GetFont().GetResourceHandle())
            oldFont = (HFONT) ::SelectObject(dc, (HFONT) GetFont().GetResourceHandle());

        GetTextMetrics(dc, &lpTextMetric);
        SIZE extentXY;
        ::GetTextExtentPoint(dc, (LPTSTR) (const wxChar *)s, s.Length(), &extentXY);
        int extentX = (int)(extentXY.cx + lpTextMetric.tmAveCharWidth);

        if (oldFont)
            ::SelectObject(dc, oldFont);

        ReleaseDC(GetHwnd(), dc);
        if (extentX > existingExtent)
            SendMessage(GetHwnd(), LB_SETHORIZONTALEXTENT, LOWORD(extentX), 0L);
    }
    else
    {
        int largestExtent = 0;
        HDC dc = GetWindowDC(GetHwnd());
        HFONT oldFont = 0;
        if (GetFont().Ok() && GetFont().GetResourceHandle())
            oldFont = (HFONT) ::SelectObject(dc, (HFONT) GetFont().GetResourceHandle());

        GetTextMetrics(dc, &lpTextMetric);
        int i;
        for (i = 0; i < m_noItems; i++)
        {
            int len = (int)SendMessage(GetHwnd(), LB_GETTEXT, i, (LONG)wxBuffer);
            wxBuffer[len] = 0;
            SIZE extentXY;
            ::GetTextExtentPoint(dc, (LPTSTR)wxBuffer, len, &extentXY);
            int extentX = (int)(extentXY.cx + lpTextMetric.tmAveCharWidth);
            if (extentX > largestExtent)
                largestExtent = extentX;
        }
        if (oldFont)
            ::SelectObject(dc, oldFont);

        ReleaseDC(GetHwnd(), dc);
        SendMessage(GetHwnd(), LB_SETHORIZONTALEXTENT, LOWORD(largestExtent), 0L);
    }
*/
}

wxSize wxListBox::DoGetBestSize()
{
    // find the widest string
    int wLine;
    int wListbox = 0;
    for ( int i = 0; i < m_noItems; i++ )
    {
        wxString str(GetString(i));
        GetTextExtent(str, &wLine, NULL);
        if ( wLine > wListbox )
            wListbox = wLine;
    }

    // give it some reasonable default value if there are no strings in the
    // list
    if ( wListbox == 0 )
        wListbox = 100;

    // the listbox should be slightly larger than the widest string
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, &GetFont());

    wListbox += 3*cx;

    int hListbox = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)*(wxMax(m_noItems, 7));

    return wxSize(wListbox, hListbox);
}

// ----------------------------------------------------------------------------
// callbacks
// ----------------------------------------------------------------------------

bool wxListBox::OS2Command(WXUINT param, WXWORD WXUNUSED(id))
{
    /*
       if (param == LBN_SELCANCEL)
       {
       event.extraLong = FALSE;
       }
     */
// TODO:
/*
    if (param == LBN_SELCHANGE)
    {
        wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_windowId);
        wxArrayInt aSelections;
        int count = GetSelections(aSelections);
        if ( count > 0 )
        {
            event.m_commandInt = aSelections[0];
            event.m_clientData = GetClientData(event.m_commandInt);
            wxString str(GetString(event.m_commandInt));
            if (str != wxT(""))
            {
                event.m_commandString = str;
            }
        }
        else
        {
            event.m_commandInt = -1;
            event.m_commandString.Empty();
        }

        event.SetEventObject( this );
        ProcessCommand(event);
        return TRUE;
    }
    else if (param == LBN_DBLCLK)
    {
        wxCommandEvent event(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, m_windowId);
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);
        return TRUE;
    }
*/
    return FALSE;
}

WXHBRUSH wxListBox::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
                               WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
// TODO:
/*
    if (GetParent()->GetTransparentBackground())
        SetBkMode((HDC) pDC, TRANSPARENT);
    else
        SetBkMode((HDC) pDC, OPAQUE);

    ::SetBkColor((HDC) pDC, RGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
    ::SetTextColor((HDC) pDC, RGB(GetForegroundColour().Red(), GetForegroundColour().Green(), GetForegroundColour().Blue()));

    wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);

    // Note that this will be cleaned up in wxApp::OnIdle, if backgroundBrush
    // has a zero usage count.
    backgroundBrush->RealizeResource();
    return (WXHBRUSH) backgroundBrush->GetResourceHandle();
*/
    return (WXHBRUSH)0;
}

// ----------------------------------------------------------------------------
// wxCheckListBox support
// ----------------------------------------------------------------------------

#if wxUSE_OWNER_DRAWN

// drawing
// -------

// space beneath/above each row in pixels
// "standard" checklistbox use 1 here, some might prefer 2. 0 is ugly.
#define OWNER_DRAWN_LISTBOX_EXTRA_SPACE    (1)

// the height is the same for all items
// TODO should be changed for LBS_OWNERDRAWVARIABLE style listboxes

// NB: can't forward this to wxListBoxItem because LB_SETITEMDATA
//     message is not yet sent when we get here!
bool wxListBox::OS2OnMeasure(WXMEASUREITEMSTRUCT *item)
{
// TODO:
/*
    // only owner-drawn control should receive this message
    wxCHECK( ((m_windowStyle & wxLB_OWNERDRAW) == wxLB_OWNERDRAW), FALSE );

    MEASUREITEMSTRUCT *pStruct = (MEASUREITEMSTRUCT *)item;

    wxDC dc;
    dc.SetHDC((WXHDC)CreateIC(wxT("DISPLAY"), NULL, NULL, 0));
    dc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_ANSI_VAR_FONT));

    pStruct->itemHeight = dc.GetCharHeight() + 2*OWNER_DRAWN_LISTBOX_EXTRA_SPACE;
    pStruct->itemWidth  = dc.GetCharWidth();

    return TRUE;
*/
    return TRUE;
}

// forward the message to the appropriate item
bool wxListBox::OS2OnDraw(WXDRAWITEMSTRUCT *item)
{
// TODO:
/*
    // only owner-drawn control should receive this message
    wxCHECK( ((m_windowStyle & wxLB_OWNERDRAW) == wxLB_OWNERDRAW), FALSE );

    DRAWITEMSTRUCT *pStruct = (DRAWITEMSTRUCT *)item;

    long data = ListBox_GetItemData(GetHwnd(), pStruct->itemID);

    wxCHECK( data && (data != LB_ERR), FALSE );

    wxListBoxItem *pItem = (wxListBoxItem *)data;

    wxDC dc;
    dc.SetHDC((WXHDC)pStruct->hDC, FALSE);
    wxRect rect(wxPoint(pStruct->rcItem.left, pStruct->rcItem.top),
            wxPoint(pStruct->rcItem.right, pStruct->rcItem.bottom));

    return pItem->OnDrawItem(dc, rect,
            (wxOwnerDrawn::wxODAction)pStruct->itemAction,
            (wxOwnerDrawn::wxODStatus)pStruct->itemState);
*/
    return FALSE;
}
#endif
    // wxUSE_OWNER_DRAWN
