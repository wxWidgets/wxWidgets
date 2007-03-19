///////////////////////////////////////////////////////////////////////////////
// Name:        src/motif/listbox.cpp
// Purpose:     wxListBox
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_LISTBOX

#include "wx/listbox.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/settings.h"
    #include "wx/arrstr.h"
#endif

#ifdef __VMS
#define XtParent XTPARENT
#define XtDisplay XTDISPLAY
#endif

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/List.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif
#include "wx/motif/private.h"

    IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)

static void wxListBoxCallback(Widget w,
                              XtPointer clientData,
                              XmListCallbackStruct * cbs);

// ----------------------------------------------------------------------------
// wxSizeKeeper
// ----------------------------------------------------------------------------

// helper class to reduce code duplication
class wxSizeKeeper
{
    int m_x, m_y;
    wxWindow* m_w;
public:
    wxSizeKeeper( wxWindow* w )
        : m_w( w )
    {
        m_w->GetSize( &m_x, &m_y );
    }

    void Restore()
    {
        int x, y;

        m_w->GetSize( &x, &y );
        if( x != m_x || y != m_y )
            m_w->SetSize( -1, -1, m_x, m_y );
    }
};

// ============================================================================
// list box control implementation
// ============================================================================

// Listbox item
wxListBox::wxListBox()
{
    m_noItems = 0;
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n, const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    if( !wxControl::CreateControl( parent, id, pos, size, style,
                                   validator, name ) )
        return false;

    m_noItems = (unsigned int)n;
    m_backgroundColour = * wxWHITE;

    Widget parentWidget = (Widget) parent->GetClientWidget();
    Display* dpy = XtDisplay(parentWidget);

    Arg args[4];
    int count = 0;
    XtSetArg( args[count], XmNlistSizePolicy, XmCONSTANT ); ++count;
    XtSetArg( args[count], XmNselectionPolicy,
              ( m_windowStyle & wxLB_MULTIPLE ) ? XmMULTIPLE_SELECT :
              ( m_windowStyle & wxLB_EXTENDED ) ? XmEXTENDED_SELECT :
                                                  XmBROWSE_SELECT );
    ++count;
    if( m_font.Ok() )
    {
        XtSetArg( args[count],
                  (String)wxFont::GetFontTag(), m_font.GetFontTypeC(dpy) );
        ++count;
    }
    if( m_windowStyle & wxLB_ALWAYS_SB )
    {
        XtSetArg( args[count], XmNscrollBarDisplayPolicy, XmSTATIC );
        ++count;
    }

    Widget listWidget =
        XmCreateScrolledList(parentWidget,
                             wxConstCast(name.mb_str(), char), args, count);

    m_mainWidget = (WXWidget) listWidget;

    Set(n, choices);

    XtManageChild (listWidget);

    wxSize best = GetBestSize();
    if( size.x != -1 ) best.x = size.x;
    if( size.y != -1 ) best.y = size.y;

    XtAddCallback (listWidget,
                   XmNbrowseSelectionCallback,
                   (XtCallbackProc) wxListBoxCallback,
                   (XtPointer) this);
    XtAddCallback (listWidget,
                   XmNextendedSelectionCallback,
                   (XtCallbackProc) wxListBoxCallback,
                   (XtPointer) this);
    XtAddCallback (listWidget,
                   XmNmultipleSelectionCallback,
                   (XtCallbackProc) wxListBoxCallback,
                   (XtPointer) this);
    XtAddCallback (listWidget,
                   XmNdefaultActionCallback,
                   (XtCallbackProc) wxListBoxCallback,
                   (XtPointer) this);

    AttachWidget (parent, m_mainWidget, (WXWidget) NULL,
                  pos.x, pos.y, best.x, best.y);

    ChangeBackgroundColour();

    return true;
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       const wxArrayString& choices,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

wxListBox::~wxListBox()
{
    if( HasClientObjectData() )
        m_clientDataDict.DestroyData();
}

void wxListBox::SetSelectionPolicy()
{
    Widget listBox = (Widget)m_mainWidget;
    Arg args[3];

    XtSetArg( args[0], XmNlistSizePolicy, XmCONSTANT );

    XtSetArg( args[1], XmNselectionPolicy,
              ( m_windowStyle & wxLB_MULTIPLE ) ? XmMULTIPLE_SELECT :
              ( m_windowStyle & wxLB_EXTENDED ) ? XmEXTENDED_SELECT :
                                                  XmBROWSE_SELECT );

    XtSetValues( listBox, args, 2 );
}

void wxListBox::DoSetFirstItem( int N )
{
    int count, length;

    if (!IsValid(N))
        return;

    XtVaGetValues ((Widget) m_mainWidget,
                    XmNvisibleItemCount, &count,
                    XmNitemCount, &length,
                    NULL);
    if ((N + count) >= length)
        N = length - count;
    XmListSetPos ((Widget) m_mainWidget, N + 1);
}

void wxListBox::Delete(unsigned int n)
{
    Widget listBox = (Widget) m_mainWidget;

    XmListDeletePos (listBox, n + 1);

    m_clientDataDict.Delete(n, HasClientObjectData());
    m_noItems --;
}

int wxListBox::DoAppend(const wxString& item)
{
    Widget listBox = (Widget) m_mainWidget;

    int n;
    XtVaGetValues (listBox, XmNitemCount, &n, NULL);
    wxXmString text( item );
    //  XmListAddItem(listBox, text, n + 1);
    XmListAddItemUnselected (listBox, text(), 0);

    // It seems that if the list is cleared, we must re-ask for
    // selection policy!!
    SetSelectionPolicy();

    m_noItems ++;

    return GetCount() - 1;
}

void wxListBox::DoSetItems(const wxArrayString& items, void** clientData)
{
    Widget listBox = (Widget) m_mainWidget;

    if( HasClientObjectData() )
        m_clientDataDict.DestroyData();

    XmString *text = new XmString[items.GetCount()];
    unsigned int i;
    for (i = 0; i < items.GetCount(); ++i)
        text[i] = wxStringToXmString (items[i]);

    if ( clientData )
        for (i = 0; i < items.GetCount(); ++i)
            m_clientDataDict.Set(i, (wxClientData*)clientData[i], false);

    XmListAddItems (listBox, text, items.GetCount(), 0);
    for (i = 0; i < items.GetCount(); i++)
        XmStringFree (text[i]);
    delete[] text;

    // It seems that if the list is cleared, we must re-ask for
    // selection policy!!
    SetSelectionPolicy();

    m_noItems = items.GetCount();
}

int wxDoFindStringInList(Widget w, const wxString& s)
{
    wxXmString str( s );
    int *positions = NULL;
    int no_positions = 0;
    bool success = XmListGetMatchPos (w, str(),
                                      &positions, &no_positions);

    if (success)
    {
        int pos = positions[0];
        if (positions)
            XtFree ((char *) positions);
        return pos - 1;
    }
    else
        return -1;
}

int wxListBox::FindString(const wxString& s, bool WXUNUSED(bCase)) const
{
    // FIXME: back to base class for not supported value of bCase

    return wxDoFindStringInList( (Widget)m_mainWidget, s );
}

void wxListBox::Clear()
{
    if (m_noItems <= 0)
        return;

    wxSizeKeeper sk( this );
    Widget listBox = (Widget) m_mainWidget;

    XmListDeleteAllItems (listBox);
    if( HasClientObjectData() )
        m_clientDataDict.DestroyData();

    sk.Restore();

    m_noItems = 0;
}

void wxListBox::DoSetSelection(int N, bool select)
{
    m_inSetValue = true;
    if (select)
    {
#if 0
        if (m_windowStyle & wxLB_MULTIPLE)
        {
            int *selections = NULL;
            int n = GetSelections (&selections);

            // This hack is supposed to work, to make it possible
            // to select more than one item, but it DOESN'T under Motif 1.1.

            XtVaSetValues ((Widget) m_mainWidget,
                           XmNselectionPolicy, XmMULTIPLE_SELECT,
                           NULL);

            int i;
            for (i = 0; i < n; i++)
                XmListSelectPos ((Widget) m_mainWidget,
                                 selections[i] + 1, False);

            XmListSelectPos ((Widget) m_mainWidget, N + 1, False);

            XtVaSetValues ((Widget) m_mainWidget,
                           XmNselectionPolicy, XmEXTENDED_SELECT,
                           NULL);
        }
        else
#endif // 0
            XmListSelectPos ((Widget) m_mainWidget, N + 1, False);

    }
    else
        XmListDeselectPos ((Widget) m_mainWidget, N + 1);

    m_inSetValue = false;
}

bool wxListBox::IsSelected(int N) const
{
    // In Motif, no simple way to determine if the item is selected.
    wxArrayInt theSelections;
    int count = GetSelections (theSelections);
    if (count == 0)
        return false;
    else
    {
        int j;
        for (j = 0; j < count; j++)
            if (theSelections[j] == N)
                return true;
    }
    return false;
}

void wxListBox::DoSetItemClientObject(unsigned int n, wxClientData* clientData)
{
    m_clientDataDict.Set(n, clientData, false);
}

wxClientData* wxListBox::DoGetItemClientObject(unsigned int n) const
{
    return m_clientDataDict.Get(n);
}

void *wxListBox::DoGetItemClientData(unsigned int n) const
{
    return (void*)m_clientDataDict.Get(n);
}

void wxListBox::DoSetItemClientData(unsigned int n, void *Client_data)
{
    m_clientDataDict.Set(n, (wxClientData*)Client_data, false);
}

// Return number of selections and an array of selected integers
int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.Empty();

    Widget listBox = (Widget) m_mainWidget;
    int *posList = NULL;
    int posCnt = 0;
    bool flag = XmListGetSelectedPos (listBox, &posList, &posCnt);
    if (flag)
    {
        if (posCnt > 0)
        {
            aSelections.Alloc(posCnt);

            int i;
            for (i = 0; i < posCnt; i++)
                aSelections.Add(posList[i] - 1);

            XtFree ((char *) posList);
            return posCnt;
        }
        else
            return 0;
    }
    else
        return 0;
}

// Get single selection, for single choice list items
int wxDoGetSelectionInList(Widget listBox)
{
    int *posList = NULL;
    int posCnt = 0;
    bool flag = XmListGetSelectedPos (listBox, &posList, &posCnt);
    if (flag)
    {
        int id = -1;
        if (posCnt > 0)
            id = posList[0] - 1;
        XtFree ((char *) posList);
        return id;
    }
    else
        return -1;
}

int wxListBox::GetSelection() const
{
    return wxDoGetSelectionInList((Widget) m_mainWidget);
}

// Find string for position
wxString wxDoGetStringInList( Widget listBox, int n )
{
    XmString *strlist;
    int count;
    XtVaGetValues( listBox,
                   XmNitemCount, &count,
                   XmNitems, &strlist,
                   NULL );
    if( n < count && n >= 0 )
        return wxXmStringToString( strlist[n] );
    else
        return wxEmptyString;
}

wxString wxListBox::GetString(unsigned int n) const
{
    return wxDoGetStringInList( (Widget)m_mainWidget, n );
}

void wxListBox::DoInsertItems(const wxArrayString& items, unsigned int pos)
{
    Widget listBox = (Widget) m_mainWidget;

    XmString *text = new XmString[items.GetCount()];
    unsigned int i;
    // Steve Hammes: Motif 1.1 compatibility
    // #if XmVersion > 1100
    // Corrected by Sergey Krasnov from Steve Hammes' code
#if XmVersion > 1001
    for (i = 0; i < items.GetCount(); i++)
        text[i] = wxStringToXmString(items[i]);
    XmListAddItemsUnselected(listBox, text, items.GetCount(), pos+1);
#else
    for (i = 0; i < items.GetCount(); i++)
    {
        text[i] = wxStringToXmString(items[i]);
        // Another Sergey correction
        XmListAddItemUnselected(listBox, text[i], pos+i+1);
    }
#endif
    for (i = 0; i < items.GetCount(); i++)
        XmStringFree(text[i]);
    delete[] text;

    // It seems that if the list is cleared, we must re-ask for
    // selection policy!!
    SetSelectionPolicy();

    m_noItems += items.GetCount();
}

void wxListBox::SetString(unsigned int n, const wxString& s)
{
    wxSizeKeeper sk( this );
    Widget listBox = (Widget) m_mainWidget;

    wxXmString text( s );

    // delete the item and add it again.
    // FIXME isn't there a way to change it in place?
    XmListDeletePos (listBox, n+1);
    XmListAddItem (listBox, text(), n+1);

    sk.Restore();
}

void wxListBox::Command (wxCommandEvent & event)
{
    if (event.GetExtraLong())
        SetSelection (event.GetInt());
    else
    {
        Deselect (event.GetInt());
        return;
    }
    ProcessCommand (event);
}

void wxListBoxCallback (Widget WXUNUSED(w), XtPointer clientData,
                        XmListCallbackStruct * cbs)
{
    wxListBox *item = (wxListBox *) clientData;

    if (item->InSetValue())
        return;

    wxEventType evtType;

    if( cbs->reason == XmCR_DEFAULT_ACTION )
        evtType = wxEVT_COMMAND_LISTBOX_DOUBLECLICKED;
    else
        evtType = wxEVT_COMMAND_LISTBOX_SELECTED;

    int n = cbs->item_position - 1;
    wxCommandEvent event (evtType, item->GetId());
    if ( item->HasClientObjectData() )
        event.SetClientObject( item->GetClientObject(n) );
    else if ( item->HasClientUntypedData() )
        event.SetClientData( item->GetClientData(n) );
    event.SetInt(n);
    event.SetExtraLong(true);
    event.SetEventObject(item);
    event.SetString( item->GetString( n ) );

    int x = -1;
    if( NULL != cbs->event && cbs->event->type == ButtonRelease )
    {
        XButtonEvent* evt = (XButtonEvent*)cbs->event;

        x = evt->x;
    }

    switch (cbs->reason)
    {
    case XmCR_MULTIPLE_SELECT:
    case XmCR_BROWSE_SELECT:
#if wxUSE_CHECKLISTBOX
        item->DoToggleItem( n, x );
#endif
    case XmCR_DEFAULT_ACTION:
        item->GetEventHandler()->ProcessEvent(event);
        break;
    case XmCR_EXTENDED_SELECT:
        switch (cbs->selection_type)
        {
        case XmINITIAL:
        case XmADDITION:
        case XmMODIFICATION:
            item->DoToggleItem( n, x );
            item->GetEventHandler()->ProcessEvent(event);
            break;
        }
        break;
    }
}

WXWidget wxListBox::GetTopWidget() const
{
    return (WXWidget) XtParent( (Widget) m_mainWidget );
}

void wxListBox::ChangeBackgroundColour()
{
    wxWindow::ChangeBackgroundColour();

    Widget parent = XtParent ((Widget) m_mainWidget);
    Widget hsb, vsb;

    XtVaGetValues (parent,
        XmNhorizontalScrollBar, &hsb,
        XmNverticalScrollBar, &vsb,
        NULL);

   /* TODO: should scrollbars be affected? Should probably have separate
    * function to change them (by default, taken from wxSystemSettings)
    */
    wxColour backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    wxDoChangeBackgroundColour((WXWidget) hsb, backgroundColour, true);
    wxDoChangeBackgroundColour((WXWidget) vsb, backgroundColour, true);

    XtVaSetValues (hsb,
        XmNtroughColor, backgroundColour.AllocColour(XtDisplay(hsb)),
        NULL);
    XtVaSetValues (vsb,
        XmNtroughColor, backgroundColour.AllocColour(XtDisplay(vsb)),
        NULL);

    // MBN: why change parent's background? It looks really ugly.
    // wxDoChangeBackgroundColour((WXWidget) parent, m_backgroundColour, true);
}

void wxListBox::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();

    Widget parent = XtParent ((Widget) m_mainWidget);
    Widget hsb, vsb;

    XtVaGetValues(parent,
                  XmNhorizontalScrollBar, &hsb,
                  XmNverticalScrollBar, &vsb,
                  NULL);

    /* TODO: should scrollbars be affected? Should probably have separate
             function to change them (by default, taken from wxSystemSettings)

        wxDoChangeForegroundColour((WXWidget) hsb, m_foregroundColour);
        wxDoChangeForegroundColour((WXWidget) vsb, m_foregroundColour);
        wxDoChangeForegroundColour((WXWidget) parent, m_foregroundColour);
    */
}

unsigned int wxListBox::GetCount() const
{
    return m_noItems;
}

#define LIST_SCROLL_SPACING 6

wxSize wxDoGetListBoxBestSize( Widget listWidget, const wxWindow* window )
{
    int max;
    Dimension spacing, highlight, xmargin, ymargin, shadow;
    int width = 0;
    int x, y;

    XtVaGetValues( listWidget,
                   XmNitemCount, &max,
                   XmNlistSpacing, &spacing,
                   XmNhighlightThickness, &highlight,
                   XmNlistMarginWidth, &xmargin,
                   XmNlistMarginHeight, &ymargin,
                   XmNshadowThickness, &shadow,
                   NULL );

    for( size_t i = 0; i < (size_t)max; ++i )
    {
        window->GetTextExtent( wxDoGetStringInList( listWidget, i ), &x, &y );
        width = wxMax( width, x );
    }

    // use some arbitrary value if there are no strings
    if( width == 0 )
        width = 100;

    // get my
    window->GetTextExtent( "v", &x, &y );

    // make it a little larger than widest string, plus the scrollbar
    width += wxSystemSettings::GetMetric( wxSYS_VSCROLL_X )
        + 2 * highlight + LIST_SCROLL_SPACING + 2 * xmargin + 2 * shadow;

    // at least 3 items, at most 10
    int height = wxMax( 3, wxMin( 10, max ) ) *
        ( y + spacing + 2 * highlight ) + 2 * ymargin + 2 * shadow;

    return wxSize( width, height );
}

wxSize wxListBox::DoGetBestSize() const
{
    return wxDoGetListBoxBestSize( (Widget)m_mainWidget, this );
}

#endif // wxUSE_LISTBOX
